#include <Arduino.h>
// Configuração dos pinos
const int pinEncoder = 2; // Pino conectado ao LM393
volatile int contador = 0; // Contador de pulsos
int estadoAnterior = LOW; // Estado anterior do pino do encoder
unsigned long tempoAnterior = 0; // Para calcular RPM
const int intervalo = 1000; // Intervalo em milissegundos para calcular a velocidade

void setup() {
  pinMode(pinEncoder, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Lê o estado atual do pino
  int estadoAtual = digitalRead(pinEncoder);
  
  // Detecta a transição de borda (LOW para HIGH)
  if (estadoAtual == HIGH && estadoAnterior == LOW) {
    contador++; // Incrementa o contador de pulsos
  }

  estadoAnterior = estadoAtual; // Atualiza o estado anterior
  
  // Calcula a velocidade a cada intervalo definido
  unsigned long tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervalo) {
    tempoAnterior = tempoAtual;

    // Supondo que o disco tem 20 pulsos por revolução
    const int pulsosPorRevolucao = 20; 
    float rpm = (contador / (float)pulsosPorRevolucao) * (60000.0 / intervalo);
    
    Serial.print("RPM: ");
    Serial.println(rpm);
    Serial.print("Contador: ");
    Serial.println(contador);
    
    // Reseta o contador
    contador = 0;
  }
}
