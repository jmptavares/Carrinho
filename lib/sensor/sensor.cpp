#include <Arduino.h>
#include "define.h"
#include "sensor.h"

static int ultimaDistanciaCM = 0; // Armazena a última distância medida pelo sensor ultrassônico
static unsigned long ultimaLeituraTempo = 0; // Armazena o tempo da última leitura do sensor ultrassônico

void inicializarSensor() { // Configura os pinos do sensor ultrassônico e inicializa o estado do pino de disparo
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  digitalWrite(TRIGGER_PIN, LOW); 
}

int lerDistanciaCM() { // Dispara o sensor ultrassônico e mede o tempo de resposta para calcular a distância em centímetros
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  
  long duracao = pulseIn(ECHO_PIN, HIGH, 25000); // Aguarda o pulso de eco por até 25 ms

  if (duracao == 0) { // Se não houver resposta dentro do tempo limite retorna 999 para indicar ausência de resposta
    return 999; 
  }

  int distancia = duracao / 58; // Converte o tempo de resposta em distância em centímetros (aproximadamente 58 microsegundos por centímetro)
  return distancia;

}

void atualizarLeituraSensor() { // Atualiza periodicamente a distância armazenada pelo módulo ultrassônico
  unsigned long agora = millis();

  if (agora - ultimaLeituraTempo >= INTERVALO_LEITURA_SENSOR) { // Se o intervalo desde a última leitura for maior ou igual ao definido, realiza uma nova
    ultimaLeituraTempo = agora;
    ultimaDistanciaCM = lerDistanciaCM();
    Serial.printf("Distância medida: %d cm\n", ultimaDistanciaCM);
  }
}

int obterUltimaDistancia() { // Retorna a distância obtida na leitura mais recente em centímetros
  return ultimaDistanciaCM; 

}
