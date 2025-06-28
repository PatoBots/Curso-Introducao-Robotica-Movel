// Robô Seguidor de Linha - Arduino Nano
// Sensores: A0-A7 (8 sensores analógicos)
// Botão: D12 (pull-down)
// Motor Driver TB6612FNG: PWMA(D5), AI1(D9), AI2(D4), PWMB(D6), BI1(D7), BI2(D8)

// Definições dos pinos
#define NUM_SEN 8
#define BUTTON_PIN 12

// Pinos do motor driver TB6612FNG
#define PWMA 5   // PWM Motor A (esquerda)
#define AI1 9    // Direção Motor A
#define AI2 4    // Direção Motor A
#define PWMB 6   // PWM Motor B (direita)
#define BI1 7    // Direção Motor B
#define BI2 8    // Direção Motor B

// Pinos dos sensores (A0 a A7)
int pinoSensores[NUM_SEN] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Variáveis para calibração
int minSensor[NUM_SEN];
int maxSensor[NUM_SEN];
int valorSensores[NUM_SEN];

// Variáveis para controle PID
float Kp = 0.0;    // Constante proporcional
float Kd = 0.0;    // Constante diferencial
float erro = 0;
float erroAnterior = 0;

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
  
}

void loop() {

  seguirLinha();

}

void seguirLinha() {
  // Ler e normalizar sensores
  lerSensores();
  
  // Calcular posição da linha
  float posicao = controleLinha();
  
  // Calcular erro (0 = centro, negativo = esquerda, positivo = direita)
  erro = posicao - 3.5; // Centro dos 8 sensores (0-7)
  
  // Controle PID
  float derivativo = erro - erroAnterior;
  float pid = Kp * erro + Kd * derivativo;
  erroAnterior = erro;
  
  int velEsquerda = pid;
  int velDireita = -pid;
  
  // Limitar velocidades
  velEsquerda = constrain(velEsquerda, velMin, velMax);
  velDireita = constrain(velDireita, velMin, velMax);
  
  // Aplicar velocidades aos motores
  acelerar(velEsquerda, velDireita);
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
    return 3.5; // Retorna centro se nenhum sensor detectar linha
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