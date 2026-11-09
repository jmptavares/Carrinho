#ifndef DEFINE_H
#define DEFINE_H

#include <Arduino.h>


// Motores
#define MOTOR_DIR_ENABLE   22
#define MOTOR_DIR_PIN1     16
#define MOTOR_DIR_PIN2     17

#define MOTOR_ESQ_ENABLE   23
#define MOTOR_ESQ_PIN1     18
#define MOTOR_ESQ_PIN2     19

// PWM 
const int PWM_FREQ = 1000;
const int PWM_RES  = 8; // 0 a 255


#define MOTOR_MAX_SPEED_LIMIT  200  
#define MOTOR_RAMP_STEP        10   
#define MOTOR_RAMP_INTERVAL_MS  15  

// Sensor ultrassônico
#define TRIGGER_PIN             32  
#define ECHO_PIN                35  
#define INTERVALO_LEITURA_SENSOR 100 

// Comunicação ESP-NOW
#define SIGNAL_TIMEOUT_MS  1000 

struct PacketDadosRecebidos {
  uint8_t xAxisValue;
  uint8_t yAxisValue;
  bool switchPressed;
};

struct PacketTelemetria {
  int distanciaCM;
};

#endif