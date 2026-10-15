#ifndef COMUNICACAO_H
#define COMUNICACAO_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "define.h"

void inicializarESPNow();
void verificarTimeoutConexao();
void enviarTelemetria(int distancia);

#endif