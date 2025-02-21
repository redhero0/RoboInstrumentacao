#include <Arduino.h>
#include <AFMotor.h>
#include <Ultrasonic.h>
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>     //INCLUSÃO DE BIBLIOTECA
#include <MFRC522.h> //INCLUSÃO DE BIBLIOTECA

Adafruit_SSD1306 display = Adafruit_SSD1306();

// Configuração do sensor ultrassônico
#define TRIG_PIN 3
#define ECHO_PIN 2

#define DHTPIN A1     // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 119*

#define FRONT_LEFT_SPEED 85
#define FRONT_RIGHT_SPEED 80
#define TURN_LEFT_SPEED 70
#define TURN_RIGHT_SPEED 70

#define FRONT_LEFT_SPEED_AUTO 86
#define FRONT_RIGHT_SPEED_AUTO 82
#define TURN_LEFT_SPEED_AUTO 80
#define TURN_RIGHT_SPEED_AUTO 80

#define SENSOR_PIN A3 // Pino para o sensor de leitura analógica

#define SS_PIN 10  // PINO SDA
#define RST_PIN A2 // PINO DE RESET
#define BUZZER_PIN 8
//int buzzer = 8;

// Objetos para os sensores
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

DHT dht(DHTPIN, DHTTYPE);

MFRC522 rfid(SS_PIN, RST_PIN); // PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

// Variáveis para os motores
AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

char t;
#define PIN_LED 3
#define TEMP_SENSOR 2

bool isAutonomousMode = false; // Indica se está no modo autônomo ou manual
unsigned long previousMillis = 0;
const long moveForwardInterval = 80;
const long turnInterval = 1000;
const long pauseInterval = 200;
const long waitBeforeForwardInterval = 200;
const long waitAfterTurnInterval = 50;

bool isTurning = false;
bool isMovingForward = false;
bool isPausing = false;
bool turnComplete = false;
bool isWaitingAfterAction = false;

unsigned int ultrassom_read = 0;
unsigned int last_ultrassom_read = 0;

void goForward();
void turnLeft();
void goBack();
void turnRight();
void stops();
void turnOnLed();
void turnOffLed();
void autonomousMode();
void manualMode();
void stopMotors(); // Função para parar os motores

void setup()
{

  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(TEMP_SENSOR, INPUT);
  digitalWrite(PIN_LED, LOW);
  pinMode(BUZZER_PIN, OUTPUT);

  left_motor.setSpeed(0);
  right_motor.setSpeed(0);
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
  dht.begin();

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // ENDEREÇO I2C 0x3C
  display.setTextColor(WHITE);
  display.setTextSize(1.2);
  display.clearDisplay();

  dht.begin();

  SPI.begin();     // INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); // INICIALIZA MFRC522
  Serial.println("Init complete!");
}

void loop()
{

  int sensorValue = analogRead(SENSOR_PIN); // Lê o valor do sensor analógico
  Serial.print("Sensor de linha: ");
  Serial.println(sensorValue);
  if (sensorValue > 850)
  {
    // Se estava no modo manual e vai para o modo autônomo
    if (!isAutonomousMode)
    {
      stopMotors();                 // Para os motores antes de mudar para o modo autônomo
      isMovingForward = false;      // Interrompe qualquer movimento
      isTurning = false;            // Interrompe qualquer giro
      isPausing = false;            // Interrompe qualquer pausa
      isWaitingAfterAction = false; // Interrompe qualquer espera
      isAutonomousMode = true;      // Ativa o modo autônomo
    }
  }
  else
  {
    // Se estava no modo autônomo e vai para o modo manual
    if (isAutonomousMode)
    {
      stopMotors();             // Para os motores antes de mudar para o modo manual
      isAutonomousMode = false; // Ativa o modo manual
    }
  }

  if (isAutonomousMode)
  {
    // Modo autônomo
    autonomousMode();
  }
  else
  {
    // Modo manual
    manualMode();
  }

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float ultrassom_read = 123.4; // Substitua pela leitura real do sensor de ultrassom

  display.clearDisplay(); // Garante que o display esteja limpo

  // Exibir todas as informações na tela ao mesmo tempo
  display.setCursor(0, 0);
  display.print("Temperatura: ");
  display.print(t);
  display.print(" C");

  display.setCursor(0, 20);
  display.print("Umidade: ");
  display.print(h);
  display.print(" %");

  display.setCursor(0, 40);
  display.print("Distancia: ");
  display.print(ultrassom_read);
  display.print(" cm");
  display.display();

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) // VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
  return;                                                         // RETORNA PARA LER NOVAMENTE

String strID = "";
for (byte i = 0; i < 4; i++)
{
  strID +=
      (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
      String(rfid.uid.uidByte[i], HEX) +
      (i != 3 ? ":" : "");
}
strID.toUpperCase();

Serial.print("Identificador (UID) da tag: "); // IMPRIME O TEXTO NA SERIAL
Serial.println(strID);                        // IMPRIME NA SERIAL O UID DA TAG RFID

// Emite um som curto no buzzer para indicar a leitura da tag
//tone(BUZZER_PIN, 1000); // Emite um tom de 1000 Hz
//delay(200);              // Som por 200 ms
//noTone(BUZZER_PIN);      // Para o som

rfid.PICC_HaltA();      // PARADA DA LEITURA DO CARTÃO
rfid.PCD_StopCrypto1(); // PARADA DA CRIPTOGRAFIA NO PCD
}

void autonomousMode()
{
  unsigned long currentMillis = millis();
  ultrassom_read = ultrasonic.read(CM);
  Serial.print("Distância: ");
  Serial.println(ultrassom_read);

  if (ultrassom_read < 40 && ultrassom_read > 5)
  {
    left_motor.setSpeed(FRONT_LEFT_SPEED_AUTO);
    right_motor.setSpeed(FRONT_RIGHT_SPEED_AUTO);
    left_motor.run(FORWARD);
    right_motor.run(FORWARD);
  }
  else if (ultrassom_read <= 5)
  {
    left_motor.setSpeed(0);
    right_motor.setSpeed(0);
    left_motor.run(RELEASE);
    right_motor.run(RELEASE);
    isMovingForward = false;
    isTurning = false;
    isPausing = false;
  }
  else
  {
    // Se a distância for maior que 40 cm, inicie o giro
    if (!isTurning && !isMovingForward && !isWaitingAfterAction)
    {
      isTurning = true;
      turnComplete = false;
      previousMillis = currentMillis;
    }

    if (isTurning && !turnComplete)
    {
      if (currentMillis - previousMillis >= pauseInterval)
      {
        // Pausa antes do giro
        if (isPausing)
        {
          left_motor.setSpeed(TURN_LEFT_SPEED_AUTO);
          right_motor.setSpeed(TURN_RIGHT_SPEED_AUTO);
          left_motor.run(FORWARD);
          right_motor.run(BACKWARD);
        }
        else
        {
          left_motor.run(RELEASE);
          right_motor.run(RELEASE);
        }
        isPausing = !isPausing;
        previousMillis = currentMillis;
      }

      if (currentMillis - previousMillis >= turnInterval)
      {
        isTurning = false;
        turnComplete = true;
        isWaitingAfterAction = true; // Aguarda pausa após o giro
        previousMillis = currentMillis;
        left_motor.run(RELEASE);
        right_motor.run(RELEASE);
      }
    }

    if (isWaitingAfterAction && (currentMillis - previousMillis >= waitAfterTurnInterval))
    {
      // Após a pausa, comece o movimento novamente
      isWaitingAfterAction = false;
      if (!isMovingForward)
      {
        isMovingForward = true;
        previousMillis = currentMillis;
      }
    }

    if (isMovingForward)
    {
      if (currentMillis - previousMillis < moveForwardInterval)
      {
        left_motor.setSpeed(FRONT_LEFT_SPEED_AUTO);
        right_motor.setSpeed(FRONT_RIGHT_SPEED_AUTO);
        left_motor.run(FORWARD);
        right_motor.run(FORWARD);
      }
      else
      {
        isMovingForward = false;
        left_motor.run(RELEASE);
        right_motor.run(RELEASE);
        Serial.println("Parando após andar por moveForwardInterval...");

        // Após o movimento, faça uma pausa
        isWaitingAfterAction = true;
        previousMillis = currentMillis;
      }
    }
  }

  last_ultrassom_read = ultrassom_read;
}

void manualMode()
{
  if (Serial.available())
  {
    t = Serial.read();
    Serial.println(t);
  }
  switch (t)
  {
  case 'F':
    goForward();
    break;
  case 'L':
    turnLeft();
    break;
  case 'B':
    goBack();
    break;
  case 'R':
    turnRight();
    break;
  case 'S':
    stops();
    break;
  case 'M':
    turnOnLed();
    break;
  case 'm':
    turnOffLed();
    break;
  default:
    break;
  }
}

void goForward()
{
  left_motor.setSpeed(FRONT_LEFT_SPEED);
  right_motor.setSpeed(FRONT_RIGHT_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(FORWARD);
}

void turnLeft()
{
  left_motor.setSpeed(TURN_LEFT_SPEED);
  right_motor.setSpeed(TURN_RIGHT_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(FORWARD);
}

void goBack()
{
  left_motor.setSpeed(FRONT_LEFT_SPEED);
  right_motor.setSpeed(FRONT_RIGHT_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(BACKWARD);
}

void turnRight()
{
  left_motor.setSpeed(TURN_LEFT_SPEED);
  right_motor.setSpeed(TURN_RIGHT_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(BACKWARD);
}

void stops()
{
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}

void turnOnLed()
{
  digitalWrite(PIN_LED, HIGH);
}

void turnOffLed()
{
  digitalWrite(PIN_LED, LOW);
}

void stopMotors()
{
  // Para os motores antes de trocar de modo
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}
