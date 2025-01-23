#include <Arduino.h>
#include <Stepper.h>
#include <Ultrasonic.h>

// Configuração do motor de passo
const int stepsPerRevolution = 2038;  // Número de passos por revolução completa
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Configuração do sensor ultrassônico
#define TRIG_PIN 12
#define ECHO_PIN 13
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

// Variáveis para rastreamento de posição
long stepperPosition = 0;                 // Posição atual em passos
float stepAngle = 360.0 / stepsPerRevolution; // Ângulo por passo (em graus)
float currentAngle = 0;                   // Posição angular atual (em graus)

// Limite de distância em cm
const float distanceThreshold = 10.0;     // Distância mínima para detectar objetos

// Limites de varredura (em graus)
const float minAngle = 0.0;
const float maxAngle = 180.0;

// Variável para direção do motor
int direction = 1;  // 1 para horário, -1 para anti-horário

void setup() {
  Serial.begin(9600);                     // Inicializa o monitor serial
  myStepper.setSpeed(40);                 // Define a velocidade (40 RPM)
  Serial.println("Iniciando varredura de 0° a 180°...");
}

void loop() {
  // Calcula o ângulo atual baseado na posição do motor
  currentAngle = stepperPosition * stepAngle;

  // **Muda a direção quando atinge os limites de 0° ou 180°**
  if (currentAngle >= maxAngle) {
    direction = -1;  // Inverte para anti-horário
    Serial.println("Limite máximo (180°) atingido. Invertendo direção para anti-horário...");
  } else if (currentAngle <= minAngle) {
    direction = 1;   // Inverte para horário
    Serial.println("Limite mínimo (0°) atingido. Invertendo direção para horário...");
  }

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

    // Move o motor de volta para a posição inicial (0°)
    Serial.println("Objeto detectado. Retornando à posição inicial...");
    while (stepperPosition != 0) {
      // Se o motor estiver em um ângulo maior que 0°, move-o para a esquerda
      if (stepperPosition > 0) {
        myStepper.step(-1); // Movimento anti-horário (de volta para 0°)
        stepperPosition--;  // Atualiza a posição
      }
      // Se o motor estiver em um ângulo menor que 0°, move-o para a direita
      else if (stepperPosition < 0) {
        myStepper.step(1);  // Movimento horário (de volta para 0°)
        stepperPosition++;  // Atualiza a posição
      }
      delay(2); // Pequena pausa para garantir leituras estáveis
    }
    Serial.println("Posição inicial (0°) alcançada.");
  }

  // Move o motor 1 passo na direção atual
  myStepper.step(direction);          // Ajusta o sentido com base na direção
  stepperPosition += direction;       // Atualiza a posição com base na direção

  delay(2); // Pequena pausa para garantir leituras estáveis
}
