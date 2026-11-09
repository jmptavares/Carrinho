#include <Arduino.h>
#include "define.h"
#include "sensor.h"

static int ultimaDistanciaCM = 0;
static unsigned long ultimaLeituraTempo = 0;

void inicializarSensor() {
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(TRIGGER_PIN, LOW); 
}

int lerDistanciaCM() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  
  long duracao = pulseIn(ECHO_PIN, HIGH, 25000);


  if (duracao == 0) {
    return 999; 
  }

  int distancia = duracao / 58;

  return distancia;

  Serial.printf("Distância medida: %d cm\n", distancia);
}

void atualizarLeituraSensor() {
  unsigned long agora = millis();

  if (agora - ultimaLeituraTempo >= INTERVALO_LEITURA_SENSOR) {
    ultimaLeituraTempo = agora;
    ultimaDistanciaCM = lerDistanciaCM();
  }
}

int obterUltimaDistancia() {
  return ultimaDistanciaCM;

}