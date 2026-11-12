#ifndef COMUNICACAO_H
#define COMUNICACAO_H

#define JOY_CENTRO        127 // Valor correspondente ao centro dos eixos do joystick (127, 127)
#define JOY_ALCANCE       127.0f // Alcance usado para normalizar os valores brutos do joystick (255 - 127 = 128, mas usamos 127 para manter simetria)
#define JOY_ZONA_MORTA    0.01f // Raio da zona morta aplicada ao joystick
#define JOY_EXPO          0.30f // Intensidade da curva exponencial aplicada aos comandos do joystick

void inicializarESPNow();
void verificarTimeoutConexao();
void enviarTelemetria(int distancia);

#endif