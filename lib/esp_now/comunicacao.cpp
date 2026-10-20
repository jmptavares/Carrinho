#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "define.h"
#include "comunicacao.h"
#include "motor.h"
#include <esp_wifi.h>

static const uint8_t macControle[] = {0x70, 0x4B, 0xCA, 0x91, 0x96, 0x3C};
static PacketDadosRecebidos dadosEntrada;
static unsigned long ultimaRecepcaoTempo = 0;
static unsigned long ultimaImpressaoJoystick = 0;
static bool modoProporcional = false;
static bool ultimoEstadoBotao = false;

static void processarComandos(uint8_t x, uint8_t y) {
  int throttle = 0;
  int steering = 0;

  const bool zonaMorta = (x == 127 && y == 127);

  if (!zonaMorta) {
    if (x <=254 && x >= 200 && y >= 120 && y <= 140) {
      throttle = MOTOR_MAX_SPEED_LIMIT; //frente
    } else if (x <= 20 && y <= 140 && y >= 120) {
      throttle = -MOTOR_MAX_SPEED_LIMIT; //trás
    } else if (x <= 140 && x >= 120 && y <= 254 && y >= 200) {
      steering = MOTOR_MAX_SPEED_LIMIT; //direita
    } else if (x >= 115 && x <= 140 && y <= 20) { 
      steering = -MOTOR_MAX_SPEED_LIMIT; //esquerda
    } 
  }

  int velEsq = throttle + steering;
  int velDir = throttle - steering;

  definirVelocidadeAlvo(velEsq, velDir);

   if (millis() - ultimaImpressaoJoystick >= 250) {
    ultimaImpressaoJoystick = millis();
    Serial.printf("Joystick RX: x=%u y=%u | throttle=%d steering=%d | motores E=%d D=%d\n",
                  static_cast<unsigned>(x), static_cast<unsigned>(y),
                  throttle, steering, velEsq, velDir);
  }
}

static void aoReceberDados(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if (mac == nullptr || memcmp(mac, macControle, sizeof(macControle)) != 0) return;
  if (incomingData == nullptr || len != sizeof(PacketDadosRecebidos)) return;

  memcpy(&dadosEntrada, incomingData, sizeof(dadosEntrada));
  ultimaRecepcaoTempo = millis();

  processarComandos(dadosEntrada.xAxisValue, dadosEntrada.yAxisValue);
}

void inicializarESPNow() {
  WiFi.mode(WIFI_STA);
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE); //Configura o canal do WiFi para 1
  
  Serial.print("MAC STA do robo: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Erro ao inicializar ESP-NOW!");
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
    pararMotoresImediatamente();
  }
}

void enviarTelemetria(int distancia) {
  PacketTelemetria telemetria;
  telemetria.distanciaCM = distancia;

  uint8_t macBroadcast[] = {0x70, 0x4B, 0xCA, 0x91, 0x96, 0x3C};
  
  if (!esp_now_is_peer_exist(macBroadcast)) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macBroadcast, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  }

  esp_now_send(macBroadcast, (uint8_t *)&telemetria, sizeof(telemetria));
}