#include <Arduino.h>
#include <AFMotor.h>
#include <Ultrasonic.h>

#define FRONT_LEFT_SPEED_AUTO 70
#define FRONT_RIGHT_SPEED_AUTO 70
#define TURN_LEFT_SPEED_AUTO 80
#define TURN_RIGHT_SPEED_AUTO 80
#define MAX_SPEED 255

#define ECHO 2
#define TRIG 3

Ultrasonic ultrasonic(TRIG, ECHO);
AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

unsigned int last_ultrassom_read = 0;
unsigned long previousMillis = 0;
unsigned long turningMillis = 0;
const long moveForwardInterval = 500;
const long turnInterval = 3000;
const long pauseInterval = 150;

bool isTurning = false;
bool isMovingForward = false;
bool isPausing = false;
bool turnComplete = false;

void setup() {
    Serial.begin(9600);
    left_motor.setSpeed(0);
    right_motor.setSpeed(0);
    left_motor.run(RELEASE);
    right_motor.run(RELEASE);
}

void loop() {
    unsigned int ultrassom_read = ultrasonic.read(CM);

    if (ultrassom_read < 45 && ultrassom_read > 5) {
        if(last_ultrassom_read >= 45)
        {
            left_motor.setSpeed(MAX_SPEED);
            left_motor.run(FORWARD);
            right_motor.setSpeed(MAX_SPEED);           
            right_motor.run(FORWARD);
            delay(5);
        }
        left_motor.setSpeed(FRONT_LEFT_SPEED_AUTO);
        left_motor.run(FORWARD);
        right_motor.setSpeed(FRONT_RIGHT_SPEED_AUTO);
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
                left_motor.run(FORWARD);
                right_motor.setSpeed(MAX_SPEED);                
                right_motor.run(FORWARD);
                delay(5);
                previousMillis = millis();
            }
            else if (millis() - turningMillis >= pauseInterval) {
                if (isPausing) {
                    left_motor.setSpeed(TURN_LEFT_SPEED_AUTO);
                    left_motor.run(FORWARD);
                    right_motor.setSpeed(TURN_RIGHT_SPEED_AUTO);
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
                left_motor.setSpeed(FRONT_LEFT_SPEED_AUTO);
                left_motor.run(FORWARD);
                right_motor.setSpeed(FRONT_RIGHT_SPEED_AUTO);                
                right_motor.run(FORWARD);
            } else {
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