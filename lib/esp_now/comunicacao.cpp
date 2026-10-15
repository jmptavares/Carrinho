#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "define.h"
#include "comunicacao.h"
#include "motor.h"

static const uint8_t macControle[] = {0x70, 0x4B, 0xCA, 0x91, 0x96, 0x3C};
static PacketDadosRecebidos dadosEntrada;
static unsigned long ultimaRecepcaoTempo = 0;
static bool modoProporcional = false;
static bool ultimoEstadoBotao = false;

static void processarComandos(uint8_t x, uint8_t y) {
  float normY = map(y, 254, 0, -1000, 1000) / 1000.0;
  float normX = map(x, 0, 254, -1000, 1000) / 1000.0;

  float suaveY = (normY < 0 ? -1 : 1) * (normY * normY);
  float suaveX = (normX < 0 ? -1 : 1) * (normX * normX);

  int throttle = suaveY * MOTOR_MAX_SPEED_LIMIT;
  int steering = suaveX * MOTOR_MAX_SPEED_LIMIT;

  if (abs(throttle) < 15) throttle = 0;
  if (abs(steering) < 15) steering = 0;

  int velEsq = throttle + steering;
  int velDir = throttle - steering;

  definirVelocidadeAlvo(velEsq, velDir);
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