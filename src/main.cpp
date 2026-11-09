#include <WiFi.h>
#include <Arduino.h>
#include "define.h"      
#include "motor.h"       
#include "sensor.h"      
#include "comunicacao.h"  

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