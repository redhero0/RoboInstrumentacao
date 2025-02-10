#include <AFMotor.h>
#include <Arduino.h>
#include <Ultrasonic.h>

// Configuração do sensor ultrassônico
#define TRIG A1
#define ECHO A2

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

AF_DCMotor right_motor(3);
AF_DCMotor left_motor(4);

void setup() 
{
	//Set initial speed of the motor & stop
	left_motor.setSpeed(80);
	right_motor.setSpeed(80);
	left_motor.run(RELEASE);
	right_motor.run(RELEASE);
}

void loop() 
{

	while(ultrasonic.read(CM) < 30 && ultrasonic.read(CM) > 7)
	{
		left_motor.setSpeed(80);
		right_motor.setSpeed(80);
		// Now turn off motor
		left_motor.run(FORWARD);
		// Now turn off motor
		right_motor.run(FORWARD);
	}
	if(ultrasonic.read(CM) <= 7)
	{
		left_motor.setSpeed(0);
		right_motor.setSpeed(0);
		// Now turn off motor
		left_motor.run(RELEASE);
		// Now turn off motor
		right_motor.run(RELEASE);
	}
	else 
	{
		left_motor.setSpeed(80);
		right_motor.setSpeed(80);
		// Turn on motor
		left_motor.run(FORWARD);
		// Turn on motor
		right_motor.run(BACKWARD);
		delay(200);
		// Now turn off motor
		left_motor.run(RELEASE);
		// Now turn off motor
		right_motor.run(RELEASE);
		delay(800);
	}


}