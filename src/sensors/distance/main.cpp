#include <Ultrasonic.h>
#include <Arduino.h>

#define ECHO 13
#define TRIG 12

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

    Serial.begin(115200); 

}

void loop(){
    Serial.print("Sensor: ");
    Serial.println(ultrasonic.read(CM));
}