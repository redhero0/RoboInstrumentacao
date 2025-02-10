char t;
#define PIN_LED 12
#define TEMP_SENSOR A1

void setup() {
  Serial.begin(9600);
  pinMode(PIN_LED, OUTPUT);
  pinMode(TEMP_SENSOR, INPUT);
  digitalWrite(PIN_LED, LOW);
}

void loop() {
  if(Serial.available()){
    t = Serial.read();
    Serial.println(t);

 }
     switch (t) {
      case 'A': goForward(); break;
        case 'I': turnLeft(); break;
        case 'R': goBack(); break;
        case 'D': turnRight(); break;
        case 'P': stops(); break; 
        case 'M': turnOnLed(); break;
        case 'm': turnOffLed(); break; 
        default: break;
    }
  float leitura = analogRead(TEMP_SENSOR);     // Lê o valor do sensor
  float tensao = leitura * (5.0 / 1023.0);   // Converte para tensão
  float temperatura = (tensao - 0.5) / 0.01; // Converte tensão para Celsius
  Serial.print("T");
  Serial.println(temperatura);
  delay(1000);  // Espera 1 segundo antes de nova leitura

}
void turnOnLed() {
    digitalWrite(PIN_LED, HIGH);
}

void turnOffLed() {
    digitalWrite(PIN_LED, LOW);
}

void goForward(){

}

void turnLeft() {

}

void goBack() {

}

void turnRight() {


}

void stops(){  


}

