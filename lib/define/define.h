#ifndef DEFINE_H
#define DEFINE_H

#include <Arduino.h>


// Pinos dos motores
#define MOTOR_DIR_ENABLE   22
#define MOTOR_DIR_PIN1     16
#define MOTOR_DIR_PIN2     17

#define MOTOR_ESQ_ENABLE   23
#define MOTOR_ESQ_PIN1     18
#define MOTOR_ESQ_PIN2     19

// Configurações de PWM (ESP32 v3.x)
const int PWM_FREQ = 1000;
const int PWM_RES  = 8; // 0 a 255

// Limites e Rampamento dos Motores
#define MOTOR_MAX_SPEED_LIMIT  200  // Limite teto absoluto de velocidade (0-255)
#define MOTOR_RAMP_STEP        10   // Variação máxima por ciclo de atualização
#define MOTOR_RAMP_INTERVAL_MS  15   // Intervalo de tempo (ms) entre cada passo da rampa

// Pinos e configurações do sensor ultrassônico
#define TRIGGER_PIN             5   // Pino Trig do HC-SR04
#define ECHO_PIN                15  // Pino Echo do HC-SR04
#define INTERVALO_LEITURA_SENSOR 100 

// Comunicação ESP-NOW
#define SIGNAL_TIMEOUT_MS  1000 // Para os motores não tiver sinal do controle

// Estrutura dos dados recebidos pelo controle
struct PacketDadosRecebidos {
  uint8_t xAxisValue;
  uint8_t yAxisValue;
  bool switchPressed;
};

// Estrutura dos dados enviados para o controle
struct PacketTelemetria {
  int distanciaCM;
};

#endif