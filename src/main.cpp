#include <WiFi.h>
#include <Arduino.h>
#include "define.h"      // Ou "config.h", dependendo do nome que usou
#include "motor.h"       // Interface dos motores
#include "sensor.h"      // Interface do ultrassônico
#include "comunicacao.h"  // Interface da comunicação ESP-NOW

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando o Robô...");

  inicializarMotores();
  inicializarSensor();
  inicializarESPNow();

  Serial.println("Robô pronto e aguardando comandos!");
}

void loop() {
  atualizarRampaMotores();

  atualizarLeituraSensor();

  verificarTimeoutConexao();

  static unsigned long ultimoEnvioTelemetria = 0;
  if (millis() - ultimoEnvioTelemetria >= 200) {
    ultimoEnvioTelemetria = millis();
    enviarTelemetria(obterUltimaDistancia());
  }
}