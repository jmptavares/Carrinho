#ifndef DEFINE_H
#define DEFINE_H

#include <Arduino.h>

// Pinagem dos motores
#define MOTOR_DIR_ENABLE   22
#define MOTOR_DIR_PIN1     16
#define MOTOR_DIR_PIN2     17

#define MOTOR_ESQ_ENABLE   23
#define MOTOR_ESQ_PIN1     18
#define MOTOR_ESQ_PIN2     19

//PWM
const int PWM_FREQ = 1000; // Frequência do PWM dos motores em Hz
const int PWM_RES  = 8; // Resolução do PWM dos motores em bits (8 bits = 0-255)

#define MOTOR_MAX_SPEED_LIMIT  200 // Limite absoluto de velocidade aplicado aos motores
#define MOTOR_RAMP_STEP        10 // Variação máxima de velocidade por etapa da rampa de aceleração dos motores
#define MOTOR_RAMP_INTERVAL_MS  15 // Intervalo mínimo entre atualizações da rampa 

// Sensor ultrassônico
#define TRIGGER_PIN             32 // Pino de disparo do sensor ultrassônico
#define ECHO_PIN                35 // Pino de leitura de eco do sensor ultrassônico
#define INTERVALO_LEITURA_SENSOR 100 // Intervalo entre leituras do sensor 
#define SIGNAL_TIMEOUT_MS  1000 // Tempo máximo sem receber comandos antes de interromper os motores

struct PacketDadosRecebidos { // Dados de joystick e botão recebidos do controle
  uint8_t xAxisValue; // Valor recebido para o eixo horizontal do joystick
  uint8_t yAxisValue; // Valor recebido para o eixo vertical do joystick
  bool switchPressed; // Indica se o botão do controle foi pressionado
};

struct PacketTelemetria { // Dados de distância enviados como telemetria
  int distanciaCM; // Distância medida em centímetros
};

#endif