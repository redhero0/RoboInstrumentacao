#include <Arduino.h>
#include <AFMotor.h>
#include <Ultrasonic.h>

// Configuração do sensor ultrassônico
#define TRIG A1
#define ECHO A2

#define LEFT_SPEED 100
#define RIGHT_SPEED 100 

// Objetos para o HC-SR04
Ultrasonic ultrasonic(TRIG, ECHO); 
unsigned int ultrassom_read = 0;

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

// Variáveis para controle de tempo
unsigned long previousMillis = 0;  
const long moveForwardInterval = 1000;  // Intervalo de tempo para andar para frente (2 segundos)
const long turnInterval = 700;          // Intervalo de tempo para girar (800 ms)
bool isTurning = false;
bool isMovingForward = false;

void setup() 
{
	// Configuração inicial da velocidade do motor e parada
	left_motor.setSpeed(LEFT_SPEED);
	right_motor.setSpeed(RIGHT_SPEED);
	left_motor.run(RELEASE);
	right_motor.run(RELEASE);
}

void loop() 
{
	unsigned long currentMillis = millis();  // Captura o tempo atual

	// Se a leitura do sensor ultrassônico for válida (distância entre 7 e 30 cm)
	if (ultrasonic.read(CM) < 30 && ultrasonic.read(CM) > 7)
	{
		left_motor.setSpeed(LEFT_SPEED);
		right_motor.setSpeed(RIGHT_SPEED);
		// Movimento para frente
		left_motor.run(FORWARD);
		right_motor.run(FORWARD);
		isMovingForward = true; // Marca que o robô está indo para frente
		previousMillis = currentMillis; // Reinicia o temporizador
	}
	else if (ultrasonic.read(CM) <= 7)
	{
		// Quando o sensor detectar um objeto muito perto
		left_motor.setSpeed(0);
		right_motor.setSpeed(0);
		// Parar os motores
		left_motor.run(RELEASE);
		right_motor.run(RELEASE);
		isMovingForward = false; // Marca que o robô parou
	}
	else 
	{
		// Caso não detecte nada (distância maior que 30 cm)
		if (!isTurning && isMovingForward) {
			// Inicia o giro após o tempo de andar para frente
			if (currentMillis - previousMillis >= moveForwardInterval) {
				left_motor.setSpeed(LEFT_SPEED);
				right_motor.setSpeed(0);
				left_motor.run(FORWARD);
				right_motor.run(RELEASE);
				isTurning = true;  // Marca que está girando
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
				previousMillis = currentMillis;  // Reinicia o temporizador para movimento
			}
		}
	}
}
