#include <Arduino.h>
#include <Stepper.h>
#include <Ultrasonic.h>

// Configuração do motor de passo
const int stepsPerRevolution = 2038; // Número de passos por revolução completa
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Configuração do sensor ultrassônico
#define TRIG_PIN 12
#define ECHO_PIN 13
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

// Variáveis para rastreamento de posição
long stepperPosition = 0; // Posição atual em passos
float stepAngle = 360.0 / stepsPerRevolution; // Ângulo por passo (em graus)
float currentAngle = 0; // Posição angular atual (em graus)

// Limite de distância em cm
const float distanceThreshold = 10.0; // Distância mínima para detectar objetos

void setup() {
  Serial.begin(9600); // Inicializa o monitor serial
  myStepper.setSpeed(10); // Define a velocidade do motor (RPM)
  Serial.println("Iniciando varredura...");
}

void loop() {
  // Faz o motor completar uma varredura de 360 graus
  for (int i = 0; i < stepsPerRevolution; i++) {
    // Lê a distância do sensor ultrassônico
    float distance = ultrasonic.read(CM);

    // Verifica se a distância está abaixo do limite
    if (distance > 0 && distance < distanceThreshold) {
      // Imprime a posição angular e a distância detectada
      Serial.print("Objeto detectado! Distancia: ");
      Serial.print(distance);
      Serial.print(" cm, Angulo: ");
      Serial.print(currentAngle);
      Serial.println(" graus");

      // Pausa o motor enquanto o objeto estiver na distância limite
      Serial.println("Pausando motor...");
      while (distance > 0 && distance < distanceThreshold) {
        distance = ultrasonic.read(CM); // Continua monitorando a distância
        delay(50); // Pequena pausa para leituras estáveis
      }
      Serial.println("Objeto não detectado. Retomando varredura...");
    }

    // Move o motor 1 passo no sentido horário
    myStepper.step(1);
    stepperPosition++;

    // Atualiza o ângulo atual
    currentAngle = stepperPosition * stepAngle;

    // Reseta a posição após uma revolução completa
    if (stepperPosition >= stepsPerRevolution) {
      stepperPosition = 0;
      Serial.println("Varredura completa. Reiniciando...");
    }

    delay(10); // Pequena pausa para garantir leituras estáveis
  }
}
