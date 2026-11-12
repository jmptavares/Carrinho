#include <WiFi.h>
#include <Arduino.h>
#include "define.h"      
#include "motor.h"       
#include "sensor.h"      
#include "comunicacao.h"  

// Inicializa a serial, os motores, o sensor e a comunicacao ESP-NOW
void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando o Robô...");

  inicializarMotores();
  inicializarSensor();
  inicializarESPNow();

  Serial.println("Robô pronto e aguardando comandos!"); 
}

// Executa as atualizacoes periódicas do carrinho
void loop() {
  atualizarRampaMotores();

  atualizarLeituraSensor();

  verificarTimeoutConexao();

  static unsigned long ultimoEnvioTelemetria = 0; 
  if (millis() - ultimoEnvioTelemetria >= 200) {
    ultimoEnvioTelemetria = millis();
    enviarTelemetria(obterUltimaDistancia()); // Envia a distancia medida pelo sensor como telemetria a cada 200 ms
  }
}