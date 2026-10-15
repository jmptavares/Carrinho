#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "define.h"

void inicializarSensor();
int lerDistanciaCM();       // Leitura pontual (em centímetros)
void atualizarLeituraSensor(); // Leitura periódica sem travar o código principal
int obterUltimaDistancia(); // Retorna o último valor lido em cm

#endif