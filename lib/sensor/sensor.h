#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include "define.h"

void inicializarSensor();
int lerDistanciaCM();       
void atualizarLeituraSensor(); 
int obterUltimaDistancia(); 

#endif