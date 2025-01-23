#include <AFMotor.h>
#include <Arduino.h>

AF_DCMotor right_motor(1);
AF_DCMotor left_motor(2);

void setup() 
{
	//Set initial speed of the motor & stop
	left_motor.setSpeed(200);
	left_motor.run(RELEASE);
}

void loop() 
{
	uint8_t i;

	// Turn on motor
	left_motor.run(FORWARD);
	// Turn on motor
	right_motor.run(FORWARD);
	
	// Accelerate from zero to maximum speed
	for (i=0; i<255; i++) 
	{
		left_motor.setSpeed(i);  
		right_motor.setSpeed(i);  
		delay(10);
	}
	
	// Decelerate from maximum speed to zero
	for (i=255; i!=0; i--) 
	{
		left_motor.setSpeed(i);
		right_motor.setSpeed(i);    
		delay(10);
	}

	// Now change motor direction
	left_motor.run(BACKWARD);
	right_motor.run(BACKWARD);
	
	// Accelerate from zero to maximum speed
	for (i=0; i<255; i++) 
	{
		left_motor.setSpeed(i);  
		right_motor.setSpeed(i);
		delay(10);
	}

	// Decelerate from maximum speed to zero
	for (i=255; i!=0; i--) 
	{
		left_motor.setSpeed(i);
		right_motor.setSpeed(i);  
		delay(10);
	}

	// Now turn off motor
	left_motor.run(RELEASE);
	// Now turn off motor
	right_motor.run(RELEASE);
	delay(1000);
}