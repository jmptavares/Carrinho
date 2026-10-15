#ifndef MOTOR_H
#define MOTOR_H

#include "define.h"

void inicializarMotores();
void definirVelocidadeAlvo(int velEsq, int velDir);
void atualizarRampaMotores();
void pararMotoresImediatamente();

#endif