#ifndef COMUNICACAO_H
#define COMUNICACAO_H

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "define.h"

//(0 a 255)   !!!! valores de (x, y) no neutro
const float X_CENTER = 127.0f;
const float Y_CENTER = 127.0f;

// Escala dos eixos !!!! (distância máxima aproximada do centro até as extremidades)
const float X_RANGE = 127.0f;
const float Y_RANGE = 127.0f;

const float DEADZONE_RADIUS = 0.15f; // Zona morta de 15% em torno do centro

void inicializarESPNow();
void verificarTimeoutConexao();
void enviarTelemetria(int distancia);

#endif