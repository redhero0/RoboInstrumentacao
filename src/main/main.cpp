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
#define TURN_LEFT_SPEED 80
#define TURN_RIGHT_SPEED 80

#define FRONT_LEFT_SPEED_AUTO 70
#define FRONT_RIGHT_SPEED_AUTO 70
#define TURN_LEFT_SPEED_AUTO 80
#define TURN_RIGHT_SPEED_AUTO 80

#define MAX_SPEED 255

#define SENSOR_PIN A3 // Pino para o sensor de leitura analógica

#define SS_PIN 10  // PINO SDA
#define RST_PIN A2 // PINO DE RESET
#define BUZZER_PIN A0
//int buzzer = 8;
unsigned long displayMillis = 0;
const long displayInterval = 3000;

// Objetos para os sensores
Ultrasonic ultrasonic(TRIG_PIN, ECHO_PIN);

DHT dht(DHTPIN, DHTTYPE);

MFRC522 rfid(SS_PIN, RST_PIN); // PASSAGEM DE PARÂMETROS REFERENTE AOS PINOS

// Variáveis para os motores
AF_DCMotor right_motor(4);
AF_DCMotor left_motor(3);

char t;
#define PIN_LED 3
#define TEMP_SENSOR 2

bool isAutonomousMode = false; // Indica se está no modo autônomo ou manual

unsigned long previousMillis = 0;
unsigned long turningMillis = 0;
const long moveForwardInterval = 200;
const long turnInterval = 3000;
const long pauseInterval = 150;

bool isTurning = false;
bool isMovingForward = false;
bool isPausing = false;
bool turnComplete = false;

unsigned int last_ultrassom_read = 0;

int tagFlag = 0;

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
void updateDisplay();
void tagIdentified ();
void gradualSpeedChange(AF_DCMotor &motor, int targetSpeed) ;

void setup()
{
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);

  left_motor.setSpeed(0);
  right_motor.setSpeed(0);
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // ENDEREÇO I2C 0x3C
  display.setTextColor(WHITE);
  display.setTextSize(1.2);
  display.clearDisplay();

  dht.begin();

  SPI.begin();     // INICIALIZA O BARRAMENTO SPI
  rfid.PCD_Init(); // INICIALIZA MFRC522


}

void loop()
{

  if (!tagFlag) {  // Só lê o sensor se a tag ainda não foi lida
    int sensorValue = analogRead(SENSOR_PIN);
    if (sensorValue > 850 && sensorValue < 1010) {
        if (!isAutonomousMode) {
            stopMotors();
            isAutonomousMode = true;
        }
    } else {
        if (isAutonomousMode) {
            stopMotors();
            isAutonomousMode = false;
        }
    }
  } else {
    // Garante que o modo manual esteja sempre ativado após leitura da tag
    isAutonomousMode = false;
  }

  if (isAutonomousMode) {
      autonomousMode();
  } else {
      manualMode();
  }

  if (!isAutonomousMode && (millis() - displayMillis >= displayInterval)) {
    displayMillis = millis();
    updateDisplay();
  }

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) // VERIFICA SE O CARTÃO PRESENTE NO LEITOR É DIFERENTE DO ÚLTIMO CARTÃO LIDO. CASO NÃO SEJA, FAZ
    return;                                                         // RETORNA PARA LER NOVAMENTE
  tagIdentified();
}

void tagIdentified ()
{
  tagFlag = 1;
  // String strID = "";
  // for (byte i = 0; i < 4; i++)
  // {
  //   strID +=
  //       (rfid.uid.uidByte[i] < 0x10 ? "0" : "") +
  //       String(rfid.uid.uidByte[i], HEX) +
  //       (i != 3 ? ":" : "");
  // }
  // strID.toUpperCase();

  rfid.PICC_HaltA();      // PARADA DA LEITURA DO CARTÃO
  rfid.PCD_StopCrypto1(); // PARADA DA CRIPTOGRAFIA NO PCD

  digitalWrite (BUZZER_PIN, LOW) ;
  delay (500) ;
  digitalWrite (BUZZER_PIN, HIGH) ;
  delay (500) ;
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}

void updateDisplay()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float ultrassom_read = ultrasonic.read(CM); // Substitua pela leitura real do sensor de ultrassom

  display.clearDisplay(); // Garante que o display esteja limpo

  // Definir tamanho da fonte
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Exibir todas as informações na tela ao mesmo tempo
  display.setCursor(0, 0);
  display.print("Temp: ");
  display.print(t);
  display.print(" C");

  display.setCursor(0, 10);
  display.print("Umid: ");
  display.print(h);
  display.print(" %");

  display.setCursor(0, 20);
  display.print("Dist: ");
  display.print(ultrassom_read);
  display.print(" cm");

  // Atualizar o display
  display.display();
}

void autonomousMode()
{
    unsigned int ultrassom_read = ultrasonic.read(CM);

    if (ultrassom_read < 45 && ultrassom_read > 5) {
        if (last_ultrassom_read >= 45) {
            left_motor.setSpeed(MAX_SPEED);            
            right_motor.setSpeed(MAX_SPEED);
            left_motor.run(FORWARD);
            right_motor.run(FORWARD);
            delay(5);
        }
        gradualSpeedChange(left_motor, FRONT_LEFT_SPEED_AUTO);       
        gradualSpeedChange(right_motor, FRONT_RIGHT_SPEED_AUTO);
        left_motor.run(FORWARD);
        right_motor.run(FORWARD);
        isTurning = false;
        turnComplete = false;
        isMovingForward = false;
    } else if (ultrassom_read <= 5) {
        left_motor.run(RELEASE);
        right_motor.run(RELEASE);
        isTurning = false;
        isMovingForward = false;
        isPausing = false;
    } else {
        if (!isTurning && !isMovingForward) {
            isMovingForward = true;
            turnComplete = false;
            isTurning = false;
            previousMillis = millis();
        }

        if (isTurning && !turnComplete) {
            if (millis() - previousMillis >= turnInterval) {
                isTurning = false;
                turnComplete = true;
                isMovingForward = true;
                left_motor.run(RELEASE);
                right_motor.run(RELEASE);
                delay(1000);
                left_motor.setSpeed(MAX_SPEED);
                right_motor.setSpeed(MAX_SPEED);
                left_motor.run(FORWARD);
                right_motor.run(FORWARD);
                delay(5);
                previousMillis = millis();
            } else if (millis() - turningMillis >= pauseInterval) {
                if (isPausing) {
                    // Aceleração gradual para a curva
                    gradualSpeedChange(left_motor, TURN_LEFT_SPEED_AUTO);
                    gradualSpeedChange(right_motor, TURN_RIGHT_SPEED_AUTO);
                    left_motor.run(FORWARD);
                    right_motor.run(BACKWARD);
                } else {
                    
                    left_motor.run(RELEASE);
                    right_motor.run(RELEASE);
                }
                isPausing = !isPausing;
                turningMillis = millis();
            }
        }

        if (isMovingForward) {
            if (millis() - previousMillis < moveForwardInterval) {
                // Aceleração gradual para o movimento para frente
                gradualSpeedChange(left_motor, FRONT_LEFT_SPEED_AUTO);
                gradualSpeedChange(right_motor, FRONT_RIGHT_SPEED_AUTO);
                left_motor.run(FORWARD);
                right_motor.run(FORWARD);
            } else {
                // Transição direta para curva
                isMovingForward = false;
                isTurning = true;
                turnComplete = false;
                left_motor.run(RELEASE);
                right_motor.run(RELEASE);
                delay(1000);
                previousMillis = millis();
                turningMillis = millis();
            }
        }
    }
    last_ultrassom_read = ultrassom_read;
}



void manualMode()
{
  if(Serial.available()){
    t = Serial.read();
    Serial.println(t);

 }
     switch (t) {
      case 'F': goForward(); break;
        case 'L': turnLeft(); break;
        case 'B': goBack(); break;
        case 'R': turnRight(); break;
        case 'S': stops(); break; 
        default: break;
    }
  delay(100);  // Espera 1 segundo antes de nova leitura

}

void gradualSpeedChange(AF_DCMotor &motor, int targetSpeed) {
  static int currentSpeedLeft = 0;
  static int currentSpeedRight = 0;

  int step = 5; // Define o incremento da aceleração

  if (&motor == &left_motor) {
      if (currentSpeedLeft < targetSpeed) {
          currentSpeedLeft += step;
          if (currentSpeedLeft > targetSpeed) currentSpeedLeft = targetSpeed;
      } else if (currentSpeedLeft > targetSpeed) {
          currentSpeedLeft -= step;
          if (currentSpeedLeft < targetSpeed) currentSpeedLeft = targetSpeed;
      }
      motor.setSpeed(currentSpeedLeft);
  } else if (&motor == &right_motor) {
      if (currentSpeedRight < targetSpeed) {
          currentSpeedRight += step;
          if (currentSpeedRight > targetSpeed) currentSpeedRight = targetSpeed;
      } else if (currentSpeedRight > targetSpeed) {
          currentSpeedRight -= step;
          if (currentSpeedRight < targetSpeed) currentSpeedRight = targetSpeed;
      }
      motor.setSpeed(currentSpeedRight);
  }
}

void goForward()
{
  left_motor.setSpeed(MAX_SPEED);
	right_motor.setSpeed(MAX_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(FORWARD);
  delay(5);
  left_motor.setSpeed(FRONT_LEFT_SPEED);
	right_motor.setSpeed(FRONT_RIGHT_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(FORWARD);
}

void turnLeft() {
  left_motor.setSpeed(MAX_SPEED);
	right_motor.setSpeed(MAX_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(FORWARD);
  delay(7);
  left_motor.setSpeed(TURN_LEFT_SPEED);
	right_motor.setSpeed(TURN_RIGHT_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(FORWARD);

}

void goBack() {
  left_motor.setSpeed(MAX_SPEED);
	right_motor.setSpeed(MAX_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(BACKWARD);
  delay(5);
  left_motor.setSpeed(FRONT_LEFT_SPEED);
	right_motor.setSpeed(FRONT_RIGHT_SPEED);
  left_motor.run(BACKWARD);
  right_motor.run(BACKWARD);
}

void turnRight() {
  left_motor.setSpeed(MAX_SPEED);
	right_motor.setSpeed(MAX_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(BACKWARD);
  delay(7);
  left_motor.setSpeed(TURN_LEFT_SPEED);
	right_motor.setSpeed(TURN_RIGHT_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(BACKWARD);

}

void stops(){  

  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}


void stopMotors()
{
  // Para os motores antes de trocar de modo
  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}