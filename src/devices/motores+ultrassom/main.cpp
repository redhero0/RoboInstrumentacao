#include <Arduino.h>
#include <AFMotor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <Ultrasonic.h>
#include <Stepper.h>

// Configuração do MPU6050
Adafruit_MPU6050 mpu;
float currentAngle = 0; // Ângulo atual do robô
float targetAngle = 0;  // Ângulo desejado para alinhar com o objeto

// Configuração dos motores de passo
const int stepsPerRevolution = 2038;  // Número de passos por revolução (ajustar conforme o motor)
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);  // Pinos do motor de passo
int stepperPosition = 0; // Contador de passos do motor de passo

// Configuração do motor DC (para movimento do robô)
AF_DCMotor leftMotor(3);
AF_DCMotor rightMotor(4);

// Configuração do sensor ultrassônico
#define TRIG_PIN 12
#define ECHO_PIN 13
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

// Limite de distância (cm) para detectar o objeto
const float distanceThreshold = 10.0;

// Função que calcula o ângulo com base na posição do motor de passo
float calculateAngle(int stepperPosition) {
  // Converte a posição do motor de passo para o ângulo correspondente
  float angle = (stepperPosition * 360.0) / stepsPerRevolution;
  return angle;
}

// Função para alinhar o robô com o ângulo desejado
void alignWithTargetAngle(float targetAngle) {
  float error; // Diferença entre o ângulo atual e o alvo
  float kP = 1.5; // Constante proporcional (ajustável)

  do {
    // Lê os dados do giroscópio
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Atualiza o ângulo atual
    currentAngle += g.gyro.z * 0.01; // Integração simples (ajustar timestep se necessário)
    if (currentAngle < 0) currentAngle += 360;
    if (currentAngle >= 360) currentAngle -= 360;

    // Calcula o erro (ângulo alvo - ângulo atual)
    error = targetAngle - currentAngle;

    // Ajusta o erro para ficar entre -180° e 180°
    if (error > 180) error -= 360;
    if (error < -180) error += 360;

    // Define a direção e a velocidade dos motores com base no erro
    int motorSpeed = abs(error) * kP;
    motorSpeed = constrain(motorSpeed, 50, 255); // Limita a velocidade mínima/máxima

    if (error > 0) {
      // Gira no sentido horário
      leftMotor.setSpeed(motorSpeed);
      rightMotor.setSpeed(motorSpeed);
      leftMotor.run(FORWARD);
      rightMotor.run(BACKWARD);
    } else {
      // Gira no sentido anti-horário
      leftMotor.setSpeed(motorSpeed);
      rightMotor.setSpeed(motorSpeed);
      leftMotor.run(BACKWARD);
      rightMotor.run(FORWARD);
    }

    // Imprime informações de depuração
    Serial.print("Erro: ");
    Serial.print(error);
    Serial.print(" | Velocidade: ");
    Serial.println(motorSpeed);

    delay(50); // Pequena pausa para estabilidade
  } while (abs(error) > 2); // Continua até o erro ser menor que 2°

  // Para os motores após alinhar
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);

  Serial.println("Alinhamento concluído!");
}

// Função para mover o robô em direção ao objeto
void moveTowardsObject() {
  Serial.println("Movendo em direção ao objeto...");
  leftMotor.run(FORWARD);
  rightMotor.run(FORWARD);
  delay(2000); // Move por 2 segundos (ajuste conforme necessário)

  // Para os motores
  leftMotor.run(RELEASE);
  rightMotor.run(RELEASE);
}

void setup() {
  Serial.begin(9600);

  // Inicializa o MPU6050
  if (!mpu.begin()) {
    Serial.println("Falha ao inicializar o MPU6050!");
    while (1);
  }
  Serial.println("MPU6050 inicializado!");

  // Configurações do MPU6050
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Configura os motores
  leftMotor.setSpeed(150);  // Velocidade inicial dos motores
  rightMotor.setSpeed(150);

  // Configura o motor de passo
  myStepper.setSpeed(10);  // Velocidade do motor de passo (ajuste conforme necessário)
}

void loop() {
  // O motor de passo irá girar até detectar um objeto
  float distance = ultrasonic.read(CM);  // Lê a distância do sensor ultrassônico

  // Gira o motor de passo enquanto não detectar um objeto dentro da distância especificada
  while (distance > distanceThreshold) {
    myStepper.step(1);  // Move o motor de passo em 1 passo por vez
    stepperPosition++;  // Incrementa o contador de passos

    // Atualiza a distância do sensor ultrassônico
    distance = ultrasonic.read(CM);

    // Garante que o contador não ultrapasse os limites de uma volta completa
    if (stepperPosition >= stepsPerRevolution) {
      stepperPosition = 0; // Zera ao completar uma volta
    } else if (stepperPosition < 0) {
      stepperPosition = stepsPerRevolution - 1; // Ajusta para evitar valores negativos
    }

    Serial.print("Distância: ");
    Serial.print(distance);
    Serial.print(" cm | Posição do motor de passo: ");
    Serial.println(stepperPosition);

    delay(10);  // Pequeno atraso para leitura estável
  }

  // Após detectar um objeto, calcular o ângulo com base na posição do motor de passo
  targetAngle = calculateAngle(stepperPosition);

  // Alinhar o robô com o ângulo detectado
  alignWithTargetAngle(targetAngle);

  // Após o alinhamento, mover os motores DC para ir em direção ao objeto
  moveTowardsObject();
}


