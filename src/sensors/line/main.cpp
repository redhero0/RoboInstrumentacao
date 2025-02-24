#include <Arduino.h>
int pinoSensor = A3; 
   
void setup(){  
  Serial.begin(9600);
  pinMode(pinoSensor, INPUT); 
}  
   
void loop(){
  Serial.println(analogRead(pinoSensor));
  if(analogRead(pinoSensor) > 680)
  {
    Serial.println("PRETO!");
  }
}