#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include "define.h"
#include "comunicacao.h"
#include "motor.h"

// Implementação da comunicação ESP-NOW e do processamento do joystick
static const uint8_t macControle[] = {0xF4, 0x65, 0x0B, 0x56, 0x6E, 0xC0}; //F4:65:0B:56:6E:C0
static PacketDadosRecebidos dadosEntrada;
static unsigned long ultimaRecepcaoTempo = 0;
static unsigned long ultimaImpressaoJoystick = 0;

// Normaliza um valor bruto de eixo do joystick para o intervalo [-1, 1]
static float normalizarEixo(uint8_t v) {
  float n = (static_cast<int>(v) - JOY_CENTRO) / JOY_ALCANCE; 
  return constrain(n, -1.0f, 1.0f);
}

// Aplica uma curva exponencial para ajustar a sensibilidade do comando do joystick
static float aplicarExpo(float v) { 
  return (1.0f - JOY_EXPO) * v + JOY_EXPO * v * v * v; // Combina o valor linear com o valor cúbico para criar a curva exponencial
}

// Converte os eixos do joystick em velocidade para os motores
static void processarComandos(uint8_t x, uint8_t y) { 
  float fx = normalizarEixo(x); // Normaliza o eixo do joystick para o intervalo [-1, 1]
  float fy = normalizarEixo(y);

  float r     = constrain(hypotf(fx, fy), 0.0f, 1.0f); // Calcula a magnitude do vetor do joystick, limitada a 1.0
  float theta = atan2f(fx, fy); // Calcula o ângulo do vetor do joystick em radianos

  if (r <= JOY_ZONA_MORTA) { // Aplica zona morta ao redor do centro para evitar movimentos indesejados
    r = 0.0f;
  } else {
    r = (r - JOY_ZONA_MORTA) / (1.0f - JOY_ZONA_MORTA);
    r = aplicarExpo(r);
  }

  float throttle = r * sinf(theta); // Componente de aceleração do joystick
  float steering = r * cosf(theta); // Componente de direção do joystick

  // Mistura de aceleração e direção para gerar velocidades proporcionais para os motores
  float esq = throttle + steering;
  float dir = throttle - steering;

  float maior = max(fabsf(esq), fabsf(dir)); // Determina o maior valor absoluto entre as velocidades dos motores
  if (maior > 0.8f) { // Limita proporcionalmente as velocidades se o maior valor exceder 0.8
    esq /= maior; 
    dir /= maior;
  }

  // Converte as velocidades normalizadas para valores inteiros de velocidade dos motores
  int velEsq = static_cast<int>(lroundf(esq * MOTOR_MAX_SPEED_LIMIT)); 
  int velDir = static_cast<int>(lroundf(dir * MOTOR_MAX_SPEED_LIMIT));

  definirVelocidadeAlvo(velEsq, velDir); // Define as velocidades alvo para os motores com base nos valores calculados

  if (millis() - ultimaImpressaoJoystick >= 250) { // Imprime os valores do joystick e as velocidades dos motores a cada 250 ms
    ultimaImpressaoJoystick = millis();
    Serial.printf("Joystick RX: x=%u y=%u | r=%.2f theta=%.0f graus | motores E=%d D=%d\n",
                  static_cast<unsigned>(x), static_cast<unsigned>(y),
                  r, theta * 180.0f / PI, velEsq, velDir); 
  }
}

// Callback chamado quando um pacote ESP-NOW é recebido
static void aoReceberDados(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (mac == nullptr || memcmp(mac, macControle, sizeof(macControle)) != 0) return; // Ignora pacotes de outros dispositivos
  if (incomingData == nullptr || len != sizeof(PacketDadosRecebidos)) return; // Ignora pacotes com tamanhos diferentes do esperado

  memcpy(&dadosEntrada, incomingData, sizeof(dadosEntrada));
  ultimaRecepcaoTempo = millis(); 

  processarComandos(dadosEntrada.xAxisValue, dadosEntrada.yAxisValue); // Processa os eixos do joystick
}

void inicializarESPNow() {
  WiFi.mode(WIFI_STA); // Inicializa o Wi-Fi em modo estação
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); // Define o canal Wi-Fi para 1

  Serial.print("MAC STA do robo: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) { 
    Serial.println("Erro ao inicializar ESP-NOW!"); // Se houver, informa algum erro no monitor serial
    return;
  }

  if (esp_now_register_recv_cb(aoReceberDados) != ESP_OK) {
    Serial.println("Erro ao registrar callback de recepcao ESP-NOW!");
    return;
  }
  Serial.println("ESP-NOW Inicializado com sucesso!");
}

void verificarTimeoutConexao() {
  if (millis() - ultimaRecepcaoTempo > SIGNAL_TIMEOUT_MS) {
    pararMotoresImediatamente(); // Para os motores quando nenhum pacote válido chega dentro do prazo
  }
}

// Envia a distância medida pelo carrinho para o controle por ESP-NOW
void enviarTelemetria(int distancia) {
  PacketTelemetria telemetria;
  telemetria.distanciaCM = distancia;

  uint8_t macBroadcast[] = {0xF4, 0x65, 0x0B, 0x56, 0x6E, 0xC0}; // Endereço MAC do controle

  if (!esp_now_is_peer_exist(macBroadcast)) {  // Adiciona o controle como peer se ainda não estiver registrado
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macBroadcast, 6);
    peerInfo.channel = 0; 
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo); 
  }

  esp_now_send(macBroadcast, (uint8_t *)&telemetria, sizeof(telemetria)); 
}