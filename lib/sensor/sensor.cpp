#include <Arduino.h>
#include "define.h"
#include "sensor.h"

static int ultimaDistanciaCM = 0;
static unsigned long ultimaLeituraTempo = 0;

void inicializarSensor() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(TRIGGER_PIN, LOW); // Garante o Trig desligado no início
}

int lerDistanciaCM() {
  // Dispara o pulso do Trigger (10 microsegundos)
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Leitura do tempo de retorno no Echo (com timeout de 25ms para não travar se não refletir nada)
  long duracao = pulseIn(ECHO_PIN, HIGH, 25000);

  // Se der timeout (retorna 0), consideramos uma distância "infinita" ou fora de alcance
  if (duracao == 0) {
    return 999; 
  }

  // Vel. do som no ar (~0.034 cm/us). Como o som vai e volta, divide por 2:
  // Distância = duracao * 0.034 / 2 -> duracao / 58.2
  int distancia = duracao / 58;

  return distancia;
}

void atualizarLeituraSensor() {
  unsigned long agora = millis();

  // Executa a leitura somente quando passar o tempo configurado (ex: 100ms)
  if (agora - ultimaLeituraTempo >= INTERVALO_LEITURA_SENSOR) {
    ultimaLeituraTempo = agora;
    ultimaDistanciaCM = lerDistanciaCM();
  }
}

int obterUltimaDistancia() {
  return ultimaDistanciaCM;
}