#include <Arduino.h>
#include <AFMotor.h>
#include <Ultrasonic.h>

#define FRONT_LEFT_SPEED 90
#define FRONT_RIGHT_SPEED 86 
#define TURN_LEFT_SPEED 80
#define TURN_RIGHT_SPEED 80 

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

// Objetos para o sensor ultrassônico
Ultrasonic ultrasonic(TRIG, ECHO);
unsigned int ultrassom_read = 0;
unsigned int last_ultrassom_read = 0;

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

// Variáveis para controle de tempo
unsigned long previousMillis = 0;  
const long moveForwardInterval = 300; // Aumentado o tempo para 500ms
const long turnInterval = 2000;       
const long pauseInterval = 150;       
const long waitBeforeForwardInterval = 300; 
const long waitAfterTurnInterval = 300; // Novo tempo de espera após giro

bool isTurning = false;
bool isMovingForward = false; // Inicialmente não se move
bool isPausing = false;  
bool isWaitingToMoveForward = false; 
bool isWaitingAfterTurn = false; // Novo estado para aguardar após o giro

void setup() 
{
    Serial.begin(9600);

    // Motores inicialmente liberados (não se movem)
    left_motor.setSpeed(0);
    right_motor.setSpeed(0);
    left_motor.run(RELEASE);
    right_motor.run(RELEASE);
}

void loop() 
{
    unsigned long currentMillis = millis();  

    // Captura a leitura do sensor ultrassônico
    ultrassom_read = ultrasonic.read(CM);
    Serial.print("Distância: ");
    Serial.println(ultrassom_read);

    // Se a leitura atual for válida (entre 5 e 30 cm) e a última leitura era "nada detectado" (>30 cm)
    if (ultrassom_read < 35 && ultrassom_read > 5)
    {
        if (!isWaitingToMoveForward && last_ultrassom_read > 30) {
            // Quando detectar o objeto pela primeira vez, entra no estado de espera
            isWaitingToMoveForward = true;
            previousMillis = currentMillis;

            left_motor.run(RELEASE);
            right_motor.run(RELEASE);
        }

        // Após o tempo de espera, o robô avança
        if (isWaitingToMoveForward && (currentMillis - previousMillis >= waitBeforeForwardInterval)) {
            left_motor.setSpeed(FRONT_LEFT_SPEED);
            right_motor.setSpeed(FRONT_RIGHT_SPEED);
            left_motor.run(FORWARD);
            right_motor.run(FORWARD);

            isMovingForward = true; 
            isTurning = false; 
            isPausing = false; 
            isWaitingToMoveForward = false;
            previousMillis = currentMillis;
        }
    }
    else if (ultrassom_read <= 5)
    {
        // Se o sensor detectar um objeto muito perto
        left_motor.setSpeed(0);
        right_motor.setSpeed(0);
        left_motor.run(RELEASE);
        right_motor.run(RELEASE);
        isMovingForward = false; 
        isTurning = false; 
        isPausing = false; 
        isWaitingToMoveForward = false;
    }
    else 
    {
        // Se não detectar nada (distância > 30 cm) e não estiver em outro estado, avança automaticamente
        if (!isMovingForward && !isTurning && !isWaitingToMoveForward && !isWaitingAfterTurn) {
            Serial.println("Nenhum obstáculo detectado. Movendo-se para frente.");
            left_motor.setSpeed(FRONT_LEFT_SPEED);
            right_motor.setSpeed(FRONT_RIGHT_SPEED);
            left_motor.run(FORWARD);
            right_motor.run(FORWARD);
            isMovingForward = true;
        }

        // Lógica de giro, se necessário
        if (!isTurning && isMovingForward) {
            if (currentMillis - previousMillis >= moveForwardInterval) {
                isTurning = true; 
                isMovingForward = false;  // O robô para para girar
                isPausing = false; 
                previousMillis = currentMillis;
            }
        } 
        else if (isTurning) {
            if (currentMillis - previousMillis >= pauseInterval) {
                if (isPausing) {
                    left_motor.setSpeed(TURN_LEFT_SPEED);
                    right_motor.setSpeed(TURN_LEFT_SPEED);
                    left_motor.run(FORWARD);
                    right_motor.run(BACKWARD);
                } else {
                    left_motor.run(RELEASE);
                    right_motor.run(RELEASE);
                }
                isPausing = !isPausing;
                previousMillis = currentMillis;
            }
            
            if (currentMillis - previousMillis >= turnInterval) {
                Serial.println("Giro completo. Pausando antes de avançar.");
                left_motor.setSpeed(0);
                right_motor.setSpeed(0);
                left_motor.run(RELEASE);
                right_motor.run(RELEASE);

                isTurning = false;
                isMovingForward = false; 
                isPausing = false;
                isWaitingAfterTurn = true; // Inicia o estado de espera após o giro
                previousMillis = currentMillis;
            }
        }

        // Lógica para aguardar após o giro e antes de seguir em frente
        if (isWaitingAfterTurn && (currentMillis - previousMillis >= waitAfterTurnInterval)) {
            isWaitingAfterTurn = false; // Finaliza o estado de espera
            isMovingForward = true;     // Avança novamente
            previousMillis = currentMillis;
        }

        // Avanço após espera (baseado em moveForwardInterval)
        if (isMovingForward && !isWaitingAfterTurn) {
            if (currentMillis - previousMillis >= moveForwardInterval) {
                left_motor.setSpeed(FRONT_LEFT_SPEED);
                right_motor.setSpeed(FRONT_RIGHT_SPEED);
                left_motor.run(FORWARD);
                right_motor.run(FORWARD);

                // Manter o movimento para frente
                isMovingForward = true;
                previousMillis = currentMillis;  // Atualizar o tempo de referência
            }
        }
    }

    // Atualiza a última leitura do sensor ultrassônico
    last_ultrassom_read = ultrassom_read;
}
