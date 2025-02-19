#include <Arduino.h>
#include <Ultrasonic.h>
#define ECHO A2
#define TRIG A1

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


void setup(){

    Serial.begin(9600); 

}

void loop(){
    Serial.print("Sensor: ");
    Serial.println(ultrasonic.read(CM));
}