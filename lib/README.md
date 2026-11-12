# Código do Carrinho — Projeto Trainee (PET Elétrica)

Código para o carrinho controlado por um ESP32. Ele recebe os comandos do joystick por ESP-NOW, controla os dois motores, mede a distância com o sensor ultrassônico e envia a telemetria de volta para o controle.

## 1. Visão Geral e Funcionamento

O ESP32 funciona como a unidade receptora dos comandos do controle. Cada pacote válido contém os valores dos eixos X e Y do joystick e o estado do botão. Os comandos dos eixos são normalizados no intervalo [-1, 1], passam por uma zona morta e por uma curva exponencial, e são combinados para determinar a velocidade e a direção de cada motor. O pressionamento do botão é recebido, mas não é usado atualmente no código, podendo ser adicionada no futuro alguma funcionalidade para ele.

As velocidades dos motores são limitadas ao intervalo de `-200` a `200` e aplicadas gradualmente, em etapas, para suavizar as mudanças. O sensor ultrassônico é lido aproximadamente a cada 100 ms. A distância mais recente é enviada ao controle por ESP-NOW aproximadamente a cada 200 ms. 

## 2. Arquitetura do Projeto

O código separa a parte principal dos módulos de motores, sensor, comunicação e definições da forma a seguir:

* **`src/main.cpp`:** Trata-se do ciclo principal do carrinho. Inicializa a serial, os motores, o sensor e a comunicação ESP-NOW. No `loop()`, atualiza a rampa dos motores, a leitura do sensor, verifica o timeout da conexão e agenda o envio da telemetria.
* **`lib/define/` (`define.h`):** Declara os pinos dos motores e do sensor ultrassônico, os parâmetros de PWM e da rampa, o timeout da comunicação e as estruturas `PacketDadosRecebidos` (eixos e botão do Joystick) e `PacketTelemetria` (distância em centímetros medida pelo sensor).
* **`lib/motores/` (`motor.cpp` / `.h`):** Configura os pinos de direção e os canais PWM dos motores. Limita as velocidades, aplica a direção de rotação e aproxima gradualmente as velocidades atuais dos valores desejados.
* **`lib/sensor/` (`sensor.cpp` / `.h`):** Dispara o sensor ultrassônico, mede a duração do eco e converte o resultado para centímetros. As leituras são atualizadas periodicamente e impressas no monitor serial.
* **`lib/esp_now/` (`comunicacao.cpp` / `.h`):** Configura o ESP32 em modo estação Wi-Fi, seleciona o canal 1 e inicializa o ESP-NOW. Valida os pacotes recebidos do MAC do controle configurado, converte os eixos do joystick em velocidades para os motores e envia a telemetria de distância ao controle.

## 3. Fluxo de Execução

### Inicialização (`setup`)

1. Inicia a comunicação Serial a 115200 baud e informa que o robô está iniciando.
2. Configura os pinos e canais PWM dos motores e os mantém parados.
3. Configura os pinos do sensor ultrassônico.
4. Inicializa o Wi-Fi e o ESP-NOW para receber os comandos do controle.
5. Escreve mensagens de inicialização no monitor serial.

### Ciclo Principal (`loop`)

1. Verifica se novos pacotes de controle foram recebidos por ESP-NOW.
2. Caso receba comandos vindos do controle, converte os eixos recebidos em velocidade alvo para cada lado e aciona os motores por meio da ponte H.
3. Atualiza a rampa de velocidade dos motores a cada 15ms aproximadamente, aproximando o PWM atual da velocidade alvo.
4. Atualiza a leitura do sensor ultrassônico a cada 100 ms aproximadamente e imprime a distância medida na serial.
5. Verifica continuamente se houve timeout dos comandos; após mais de 1000 ms sem um pacote válido, para os motores imediatamente. 
6. A cada 200 ms, envia ao controle a última distância medida.
