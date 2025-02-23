#include <Arduino.h>
#include <AFMotor.h>
#include "DHT.h"

#include <Ultrasonic.h>

// Configuração do sensor ultrassônico
#define TRIG A1
#define ECHO A2

#define FRONT_LEFT_SPEED 85
#define FRONT_RIGHT_SPEED 80 
#define TURN_LEFT_SPEED 70
#define TURN_RIGHT_SPEED 70 
#define MAX_SPEED 255
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
unsigned int ultrassom_read = 0;
 
#define DHTPIN 2 // pino que estamos conectado
#define DHTTYPE DHT11 // DHT 11
 
// Conecte pino 1 do sensor (esquerda) ao +5V
// Conecte pino 2 do sensor ao pino de dados definido em seu Arduino
// Conecte pino 4 do sensor ao GND
// Conecte o resistor de 10K entre pin 2 (dados) 
// e ao pino 1 (VCC) do sensor
DHT dht(DHTPIN, DHTTYPE);
char t;
#define PIN_LED 3
#define TEMP_SENSOR 2
AF_DCMotor right_motor(4);
AF_DCMotor left_motor(3);

void goForward();
void turnLeft(); 
void goBack(); 
void turnRight(); 
void stops(); 
void turnOnLed(); 
void turnOffLed();

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(TEMP_SENSOR, INPUT);
  digitalWrite(PIN_LED, LOW);
  //Set initial speed of the motor & stop
	left_motor.setSpeed(80);
	right_motor.setSpeed(FRONT_RIGHT_SPEED);
	left_motor.run(RELEASE);
	right_motor.run(RELEASE);
  dht.begin();
}

void loop() {
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
        case 'M': turnOnLed(); break;
        case 'm': turnOffLed(); break; 
        default: break;
    }
  float temperatura = dht.readTemperature();
  float d = ultrasonic.read(CM);
  Serial.print("D");
  Serial.println(d);
  Serial.print("T");
  Serial.println(temperatura);
  delay(100);  // Espera 1 segundo antes de nova leitura

}
void turnOnLed() {
    digitalWrite(PIN_LED, HIGH);
}

void turnOffLed() {
    digitalWrite(PIN_LED, LOW);
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
  delay(5);
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
  delay(5);
  left_motor.setSpeed(TURN_LEFT_SPEED);
	right_motor.setSpeed(TURN_RIGHT_SPEED);
  left_motor.run(FORWARD);
  right_motor.run(BACKWARD);

}

void stops(){  

  left_motor.run(RELEASE);
  right_motor.run(RELEASE);
}

