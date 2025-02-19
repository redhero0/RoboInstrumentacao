#include <Arduino.h>
int pinoSensor = 8; 
   
void setup(){  
  Serial.begin(9600);
  pinMode(pinoSensor, INPUT); 
}  
   
void loop(){
  if ((pinoSensor) == LOW)
    Serial.println("LOW");
  else
    Serial.println("HIGH");
}