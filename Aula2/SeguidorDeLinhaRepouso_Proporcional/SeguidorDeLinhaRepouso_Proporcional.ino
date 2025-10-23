// Robô Seguidor de Linha Repouso com Controle Proporcional - Arduino Nano
// Sensores: A1-A6 (6 sensores analógicos)
// Botão: D12 (pull-down)
// Motor Driver TB6612FNG: PWMA(D5), AI1(D9), AI2(D4), PWMB(D6), BI1(D7), BI2(D8)

// Definições dos pinos
#define NUM_SEN 6
#define BUTTON_PIN 12

// Pinos do motor driver TB6612FNG
#define PWMA 5   // PWM Motor A (esquerda)
#define AI1 9    // Direção Motor A
#define AI2 4    // Direção Motor A
#define PWMB 6   // PWM Motor B (direita)
#define BI1 7    // Direção Motor B
#define BI2 8    // Direção Motor B

// Pinos dos sensores (A1 a A6)
int pinoSensores[NUM_SEN] = {A1, A2, A3, A4, A5, A6};

// Variáveis para calibração
int minSensor[NUM_SEN];
int maxSensor[NUM_SEN];
int valorSensores[NUM_SEN];

// Variáveis para controle PID
float Kp = 0.0;    // Constante proporcional
float erro = 0;

// Velocidades dos motores
int velBase = 40;     // Velocidade base (0-255)
int velMax = 255;      // Velocidade máxima
int velMin = 0;        // Velocidade mínima

// Estados do programa
enum Estado {
  ESPERANDO,
  CALIBRANDO,
  CORRENDO
};

Estado estadoAtual = ESPERANDO;
bool botaoAtual = false;
bool botaoEstadoAnteior = false;

void setup() {
  Serial.begin(9600);
  
  // Configuração dos pinos
  pinMode(BUTTON_PIN, INPUT);
  
  // Configuração dos pinos do motor
  pinMode(PWMA, OUTPUT);
  pinMode(AI1, OUTPUT);
  pinMode(AI2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BI1, OUTPUT);
  pinMode(BI2, OUTPUT);
  
  // Parar motores inicialmente
  pararMotores();
  
  // Inicializar valores de calibração
  for (int i = 0; i < NUM_SEN; i++) {
    minSensor[i] = 1023;
    maxSensor[i] = 0;
  }
  
  Serial.println("Sistema iniciado. Pressione o botão para calibrar.");
}

void loop() {
  // Leitura do botão
  bool estadoBotao = digitalRead(BUTTON_PIN);
  
  // Detecção de borda de subida do botão
  if (estadoBotao && !botaoEstadoAnteior) {
    botaoAtual = true;
    delay(50); // Debounce
  }
  botaoEstadoAnteior = estadoBotao;
  
  // Máquina de estados
  switch (estadoAtual) {
    case ESPERANDO:
      if (botaoAtual) {
        estadoAtual = CALIBRANDO;
        botaoAtual = false;
        Serial.println("Iniciando calibração...");
        delay(500);
        calibrarSensores();
      }
      break;
      
    case CALIBRANDO:
      estadoAtual = CORRENDO;
      Serial.println("Calibração concluída. Iniciando corrida!");
      delay(3000); // Depois de 3s inicia a corrida 
      break;
      
    case CORRENDO:
      if (botaoAtual) {
        estadoAtual = ESPERANDO;
        botaoAtual = false;
        pararMotores();
        Serial.println("Parando robô. Pressione o botão novamente para recalibrar.");
      } else {
        seguirLinha();
      }
      break;
  }
}

void calibrarSensores() {
  Serial.println("Movendo robô para calibração...");
  
  // Calibração por 3 segundos girando o robô
  unsigned long tempoInicial = millis();
  
  while (millis() - tempoInicial < 3000) {
    // Girar o robô lentamente para a direita
    //acelerar(50, -50);
    
    // Ler sensores e atualizar valores min/max
    for (int i = 0; i < NUM_SEN; i++) {
      int faixa = analogRead(pinoSensores[i]);
      if (faixa < minSensor[i]) {
        minSensor[i] = faixa;
      }
      if (faixa > maxSensor[i]) {
        maxSensor[i] = faixa;
      }
    }
    delay(10);
  }
  
  // Girar para o outro lado
  tempoInicial = millis();
  while (millis() - tempoInicial < 3000) {
    // Girar o robô lentamente para a esquerda
    //acelerar(-50, 50);
    
    // Ler sensores e atualizar valores min/max
    for (int i = 0; i < NUM_SEN; i++) {
      int faixa = analogRead(pinoSensores[i]);
      if (faixa < minSensor[i]) {
        minSensor[i] = faixa;
      }
      if (faixa > maxSensor[i]) {
        maxSensor[i] = faixa;
      }
    }
    delay(10);
  }
  
  pararMotores();
  
  // Mostrar valores de calibração
  Serial.println("Valores de calibração:");
  for (int i = 0; i < NUM_SEN; i++) {
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(": Min=");
    Serial.print(minSensor[i]);
    Serial.print(", Max=");
    Serial.println(maxSensor[i]);
  }
  
  delay(1000);
}

void seguirLinha() {
  // Ler e normalizar sensores
  lerSensores();
  
  // Calcular posição da linha
  float posicao = controleLinha();
  
  // Calcular erro (0 = centro, negativo = esquerda, positivo = direita)
  erro = posicao - 2.5; // Centro dos 6 sensores (0-5)
  
  // Controle PID
  float pid = Kp * erro;
  
  // Calcular velocidades dos motores
  int velEsquerda = pid;
  int velDireita = -pid;
  
  // Limitar velocidades
  velEsquerda = constrain(velEsquerda, velMin, velMax);
  velDireita = constrain(velDireita, velMin, velMax);
  
  // Aplicar velocidades aos motores
  acelerar(velEsquerda, velDireita);
  
  // Debug (opcional)
/*
  Serial.print("Posição: ");
  Serial.print(posicao);
  Serial.print(" | Erro: ");
  Serial.print(erro);
  Serial.print(" | Correção: ");
  Serial.print(correcao);
  Serial.print(" | Motores: L=");
  Serial.print(velEsquerda);
  Serial.print(" R=");
  Serial.println(velDireita);
*/
}

void lerSensores() {
  for (int i = 0; i < NUM_SEN; i++) {
    int leitura = analogRead(pinoSensores[i]);
    // Normalizar entre 0 e 1000
    valorSensores[i] = map(leitura, minSensor[i], maxSensor[i], 0, 1000);
    valorSensores[i] = constrain(valorSensores[i], 0, 1000);
  }
}

float controleLinha() {
  long num = 0;
  long den = 0;
  
  for (int i = 0; i < NUM_SEN; i++) {
    num += (long)valorSensores[i] * i * 1000;
    den += valorSensores[i];
  }
  
  if (den == 0) {
    return 2.5; // Retorna centro se nenhum sensor detectar linha
  }
  
  return (float)num / den / 1000.0;
}

void acelerar(int velEsquerda, int velDireita) {
  // Motor esquerdo (canal A)
  if (velEsquerda >= 0) {
    digitalWrite(AI1, HIGH);
    digitalWrite(AI2, LOW);
    analogWrite(PWMA, velEsquerda);
  } else {
    digitalWrite(AI1, LOW);
    digitalWrite(AI2, HIGH);
    analogWrite(PWMA, -velEsquerda);
  }
  
  // Motor direito (canal B)
  if (velDireita >= 0) {
    digitalWrite(BI1, HIGH);
    digitalWrite(BI2, LOW);
    analogWrite(PWMB, velDireita);
  } else {
    digitalWrite(BI1, LOW);
    digitalWrite(BI2, HIGH);
    analogWrite(PWMB, -velDireita);
  }
}

void pararMotores() {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
  digitalWrite(AI1, LOW);
  digitalWrite(AI2, LOW);
  digitalWrite(BI1, LOW);
  digitalWrite(BI2, LOW);
}