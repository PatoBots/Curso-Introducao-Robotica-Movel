//configuração dos motores
const int motor1PWM = 9;  // Pino PWM para o motor 1
const int motor1DirA = 8; // Pino de controle de direção A do motor 1
const int motor1DirB = 7; // Pino de controle de direção B do motor 1

const int motor2PWM = 10;  // Pino PWM para o motor 2
const int motor2DirA = 12; // Pino de controle de direção A do motor 2
const int motor2DirB = 11; // Pino de controle de direção B do motor 2

// Definindo os pinos das entradas analógicas
const int pinA0 = A0;
const int pinA1 = A1;
const int pinA2 = A2;

long int faixa=0;
long int ref=1000;
long int erro=0;
long int valorA0, valorA1, valorA2;
long int num,den;

long int delta;

int velMotor_dir = 100;
int velMotor_esq = 100;
float Kp=0.1;
float vel=100;
int PWMD,PWME;

void setup() {
  Serial.begin(9600);
  
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1DirA, OUTPUT);
  pinMode(motor1DirB, OUTPUT);
  
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2DirA, OUTPUT);
  pinMode(motor2DirB, OUTPUT);
}

void loop() {

  // Lê os valores das entradas analógicas
  valorA0 = analogRead(pinA0);
  valorA1 = analogRead(pinA1);
  valorA2 = analogRead(pinA2);

  if((valorA0 > 460) && (valorA1 > 460) && (valorA2 > 460)){
    digitalWrite(motor1DirA, LOW);
    digitalWrite(motor1DirB, LOW);
    digitalWrite(motor2DirA, LOW);
    digitalWrite(motor2DirB, LOW);
  }else{
    //Calculo para ver a direção que o carrinho deve ajustar
  num = 0*valorA0+1000*valorA1+2000*valorA2;
  den = valorA0+valorA1+valorA2;

  faixa = num/den;

  erro=ref-faixa;


    delta=Kp*(float)erro;
    //A velocidade não pode ser maior que 255
    PWMD=vel-delta;
    PWME=vel+delta;
    if (PWMD<0) PWMD=0;
    if (PWME>255) PWME=255;
    if (PWME<0) PWME=0;
    if (PWMD>255) PWMD=255;
   
    // Movimenta o motor da direita para a frente
    digitalWrite(motor1DirA, HIGH);
    digitalWrite(motor1DirB, LOW);
    analogWrite(motor1PWM, PWMD);

    // Movimenta o motor da esquerda para a frente
    digitalWrite(motor2DirA, HIGH);
    digitalWrite(motor2DirB, LOW);
    analogWrite(motor2PWM, PWME);
  }

  

      // Mostra os valores na porta serial
  //Serial.print("Valor A0: ");
  //Serial.print(valorA0);
  //Serial.print("   Valor A1: ");
  //Serial.print(valorA1);
  //Serial.print("   Valor A2: ");
  //Serial.println(valorA2);

  //Serial.print("num: ");
  //Serial.print(num);
  //Serial.print("   den: ");
  //Serial.print(den);
  //Serial.print("   faixa: ");
  //Serial.print(faixa);
  //Serial.print("     erro: ");
  //Serial.println(erro);
  //Serial.print("     delta: ");
  //Serial.println(delta); 
  

  //delay(1500);

}