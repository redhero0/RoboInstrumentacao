#include <Arduino.h>
#include <AFMotor.h>
#include <Ultrasonic.h>

#define ECHO 2
#define TRIG 3
/*
------------------  
|  HC-SR04 (1)   |   
------------------  
|  VCC   |  +5V  | 
|  GND   |  GND  |   
|  TRIG  |  13   |  
|  ECHO  |  12   |   

*/


// objects for the hc-sr04
Ultrasonic ultrasonic(TRIG, ECHO);
#define LEFT_SPEED 100
#define RIGHT_SPEED 100 
unsigned int  ultrassom_read = 0;

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

// Variáveis para controle de tempo
unsigned long previousMillis = 0;  
const long moveForwardInterval = 500;  // Tempo para andar para frente (1s)
const long turnInterval = 1500;         // Tempo para girar (1.5s)
bool isTurning = false;
bool isMovingForward = false;

void setup() 
{
    Serial.begin(9600); // Inicializa comunicação serial para depuração

    // Configuração inicial da velocidade do motor e parada
    left_motor.setSpeed(LEFT_SPEED);
    right_motor.setSpeed(RIGHT_SPEED);
    left_motor.run(RELEASE);
    right_motor.run(RELEASE);
}

void loop() 
{
    unsigned long currentMillis = millis();  // Captura o tempo atual

    // Captura a leitura do sensor ultrassônico
    ultrassom_read = ultrasonic.read(CM);
    Serial.print("Distância: ");
    Serial.println(ultrasonic.read(CM)); // Exibe a distância para depuração

    // Se a leitura do sensor ultrassônico for válida (distância entre 7 e 30 cm)
    if (ultrassom_read < 30 && ultrassom_read > 7)
    {
        left_motor.setSpeed(LEFT_SPEED);
        right_motor.setSpeed(RIGHT_SPEED);
        // Movimento para frente
        left_motor.run(FORWARD);
        right_motor.run(FORWARD);
        isMovingForward = true; // Marca que o robô está indo para frente
        isTurning = false; // Garante que ele não fique preso girando
        previousMillis = currentMillis; // Reinicia o temporizador
    }
    else if (ultrassom_read <= 7)
    {
        // Quando o sensor detectar um objeto muito perto
        left_motor.setSpeed(0);
        right_motor.setSpeed(0);
        // Parar os motores
        left_motor.run(RELEASE);
        right_motor.run(RELEASE);
        isMovingForward = false; // Marca que o robô parou
        isTurning = false; // Para qualquer tentativa de giro
    }
    else 
    {
        // Caso não detecte nada (distância maior que 30 cm)
        if (!isTurning && isMovingForward) {
            // Inicia o giro após o tempo de andar para frente
            if (currentMillis - previousMillis >= moveForwardInterval) {
                left_motor.setSpeed(70);
                right_motor.setSpeed(0);
                left_motor.run(FORWARD);
                right_motor.run(RELEASE);
                isTurning = true;  // Marca que está girando
                isMovingForward = false; // Interrompe o movimento para frente
                previousMillis = currentMillis;  // Armazena o tempo inicial do giro
            }
        } 
        else if (isTurning) {
            // Verifica se o tempo de giro já passou
            if (currentMillis - previousMillis >= turnInterval) {
                // Tempo expirado, agora move para frente novamente
                left_motor.setSpeed(LEFT_SPEED);
                right_motor.setSpeed(RIGHT_SPEED);
                left_motor.run(FORWARD);
                right_motor.run(FORWARD);
                isTurning = false;  // Marca que parou de girar
                isMovingForward = true; // Continua o movimento
                previousMillis = currentMillis;  // Reinicia o temporizador para movimento
            }
        }
        else {
            // Caso nenhuma condição seja atendida, mantém o robô em movimento
            left_motor.setSpeed(LEFT_SPEED);
            right_motor.setSpeed(RIGHT_SPEED);
            left_motor.run(FORWARD);
            right_motor.run(FORWARD);
            isMovingForward = true;
            previousMillis = currentMillis;
        }
    }
}
