#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <math.h>
#include "define.h"
#include "comunicacao.h"
#include "motor.h"

static const uint8_t macControle[] = {0xF4, 0x65, 0x0B, 0x56, 0x6E, 0xC0}; //F4:65:0B:56:6E:C0
static PacketDadosRecebidos dadosEntrada;
static unsigned long ultimaRecepcaoTempo = 0;
static unsigned long ultimaImpressaoJoystick = 0;

static float normalizarEixo(uint8_t v) {
  float n = (static_cast<int>(v) - JOY_CENTRO) / JOY_ALCANCE;
  return constrain(n, -1.0f, 1.0f);
}

static float aplicarExpo(float v) {
  return (1.0f - JOY_EXPO) * v + JOY_EXPO * v * v * v;
}

static void processarComandos(uint8_t x, uint8_t y) {
  float fx = normalizarEixo(x);
  float fy = normalizarEixo(y);

  float r     = constrain(hypotf(fx, fy), 0.0f, 1.0f);
  float theta = atan2f(fx, fy);

  if (r <= JOY_ZONA_MORTA) {
    r = 0.0f;
  } else {
    r = (r - JOY_ZONA_MORTA) / (1.0f - JOY_ZONA_MORTA);
    r = aplicarExpo(r);
  }

  float throttle = r * sinf(theta);
  float steering = r * cosf(theta);

  float esq = throttle + steering;
  float dir = throttle - steering;

  float maior = max(fabsf(esq), fabsf(dir));
  if (maior > 0.8f) {
    esq /= maior;
    dir /= maior;
  }

  int velEsq = static_cast<int>(lroundf(esq * MOTOR_MAX_SPEED_LIMIT));
  int velDir = static_cast<int>(lroundf(dir * MOTOR_MAX_SPEED_LIMIT));

  definirVelocidadeAlvo(velEsq, velDir);

  if (millis() - ultimaImpressaoJoystick >= 250) {
    ultimaImpressaoJoystick = millis();
    Serial.printf("Joystick RX: x=%u y=%u | r=%.2f theta=%.0f graus | motores E=%d D=%d\n",
                  static_cast<unsigned>(x), static_cast<unsigned>(y),
                  r, theta * 180.0f / PI, velEsq, velDir);
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
  esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);

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

  uint8_t macBroadcast[] = {0xF4, 0x65, 0x0B, 0x56, 0x6E, 0xC0};

  if (!esp_now_is_peer_exist(macBroadcast)) {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, macBroadcast, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);
  }

  esp_now_send(macBroadcast, (uint8_t *)&telemetria, sizeof(telemetria));
}