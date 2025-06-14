// Definição dos pinos - Motor A (ESQUERDA)
#define PWMA 5     // Controle PWM do motor A (velocidade)
#define AI1  9     // Controle de direção 1
#define AI2  4     // Controle de direção 2

// Definição dos pinos - Motor B (DIREITA)
#define PWMB 6     // Controle PWM do motor B (velocidade)
#define BI1  7     // Controle de direção 1
#define BI2  8     // Controle de direção 2

#define STBY 3     // Pino Standby (habilita/desabilita a ponte H)

void setup() {
  // Configura todos os pinos como saída
  pinMode(PWMA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);

  pinMode(STBY, OUTPUT);

  // Habilita a ponte H (tira do modo standby)
  digitalWrite(STBY, HIGH);
}

void loop() {
  // 1. MOVER PARA FRENTE (ambos motores no mesmo sentido)
  // Motor esquerdo (A)
  digitalWrite(AI1, HIGH);  // Define direção para frente
  digitalWrite(AI2, LOW);   // O outro pino deve ficar LOW
  analogWrite(PWMA, 150);   // Velocidade média (~60% da máxima)
  
  // Motor direito (B)
  digitalWrite(BI1, HIGH);  // Mesma direção do motor A
  digitalWrite(BI2, LOW);
  analogWrite(PWMB, 150);
  delay(2000);              // Move por 2 segundos

  // 2. GIRO 180° ESQUERDA (roda esquerda para trás, direita para frente)
  // Motor esquerdo para trás
  digitalWrite(AI1, LOW);   // Inverte a direção
  digitalWrite(AI2, HIGH);
  analogWrite(PWMA, 150);
  
  // Motor direito continua para frente
  digitalWrite(BI1, HIGH);
  digitalWrite(BI2, LOW);
  analogWrite(PWMB, 150);
  delay(1000);              // Tempo de giro (ajustar conforme necessário)

  // 3. MOVER PARA FRENTE NOVAMENTE
  // Ambos motores para frente
  digitalWrite(AI1, HIGH);
  digitalWrite(AI2, LOW);
  analogWrite(PWMA, 150);
  digitalWrite(BI1, HIGH);
  digitalWrite(BI2, LOW);
  analogWrite(PWMB, 150);
  delay(2000);

  // 4. GIRO 180° DIREITA (roda direita para trás, esquerda para frente)
  // Motor esquerdo para frente
  digitalWrite(AI1, HIGH);
  digitalWrite(AI2, LOW);
  analogWrite(PWMA, 150);
  
  // Motor direito para trás
  digitalWrite(BI1, LOW);   // Inverte a direção
  digitalWrite(BI2, HIGH);
  analogWrite(PWMB, 150);
  delay(1000);

  // 5. PARADA COMPLETA
  analogWrite(PWMA, 0);     // Desliga PWM (velocidade zero)
  analogWrite(PWMB, 0);
  
  while (1); // Loop infinito para parar a execução
  // Remova esta linha se quiser que a sequência se repita
}