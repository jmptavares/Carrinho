#include <Arduino.h>
#include "motor.h"
#include "define.h"

#define CANAL_PWM_DIR 0
#define CANAL_PWM_ESQ 1

static int alvoEsq = 0;
static int alvoDir = 0;

static int atualEsq = 0;
static int atualDir = 0;

static unsigned long ultimaAtualizacaoRampa = 0;

void inicializarMotores() {
  pinMode(MOTOR_DIR_PIN1, OUTPUT);
  pinMode(MOTOR_DIR_PIN2, OUTPUT);
  pinMode(MOTOR_ESQ_PIN1, OUTPUT);
  pinMode(MOTOR_ESQ_PIN2, OUTPUT);

  ledcSetup(CANAL_PWM_DIR, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR_DIR_ENABLE, CANAL_PWM_DIR);

  ledcSetup(CANAL_PWM_ESQ, PWM_FREQ, PWM_RES);
  ledcAttachPin(MOTOR_ESQ_ENABLE, CANAL_PWM_ESQ);

  pararMotoresImediatamente();
}

static void aplicarSaidaMotor(int pin1, int pin2, int canalPWM, int velocidade) {
  velocidade = constrain(velocidade, -MOTOR_MAX_SPEED_LIMIT, MOTOR_MAX_SPEED_LIMIT);

  if (velocidade > 0) {
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
  } else if (velocidade < 0) {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
  } else {
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
  }

  ledcWrite(canalPWM, abs(velocidade));
}

void definirVelocidadeAlvo(int velEsq, int velDir) {
  // Limites de segurança
  alvoEsq = constrain(velEsq, -MOTOR_MAX_SPEED_LIMIT, MOTOR_MAX_SPEED_LIMIT);
  alvoDir = constrain(velDir, -MOTOR_MAX_SPEED_LIMIT, MOTOR_MAX_SPEED_LIMIT);
}

void atualizarRampaMotores() {
  unsigned long agora = millis();
  
  if (agora - ultimaAtualizacaoRampa < MOTOR_RAMP_INTERVAL_MS) {
    return; // Aguarda o intervalo da rampa
  }
  ultimaAtualizacaoRampa = agora;


  if (atualEsq < alvoEsq) {
    atualEsq = min(atualEsq + MOTOR_RAMP_STEP, alvoEsq);
  } else if (atualEsq > alvoEsq) {
    atualEsq = max(atualEsq - MOTOR_RAMP_STEP, alvoEsq);
  }

  if (atualDir < alvoDir) {
    atualDir = min(atualDir + MOTOR_RAMP_STEP, alvoDir);
  } else if (atualDir > alvoDir) {
    atualDir = max(atualDir - MOTOR_RAMP_STEP, alvoDir);
  }

  // Envia as velocidades calculadas para a ponte H
  aplicarSaidaMotor(MOTOR_ESQ_PIN1, MOTOR_ESQ_PIN2, CANAL_PWM_ESQ, atualEsq);
  aplicarSaidaMotor(MOTOR_DIR_PIN1, MOTOR_DIR_PIN2, CANAL_PWM_DIR, atualDir);
}

void pararMotoresImediatamente() {
  alvoEsq = 0;
  alvoDir = 0;
  atualEsq = 0;
  atualDir = 0;
  
  digitalWrite(MOTOR_DIR_PIN1, LOW);
  digitalWrite(MOTOR_DIR_PIN2, LOW);
  digitalWrite(MOTOR_ESQ_PIN1, LOW);
  digitalWrite(MOTOR_ESQ_PIN2, LOW);
  
  ledcWrite(CANAL_PWM_DIR, 0);
  ledcWrite(CANAL_PWM_ESQ, 0);
}