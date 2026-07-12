#include <AFMotor.h>
#include <RotaryEncoder.h>
#include <Servo.h>

// registro velocidade
unsigned long instanteContagemTeste = 0;
unsigned long instanteContagem = 0;
int TicksPorRotacao = 180;
int pulsoInicialEsq = 0;
int pulsoInicialDir = 0;
int pulsosNoSegundoEsq = 0;
int pulsosNoSegundoDir = 0;
int velocidade = 0;
int velocidadeDir = 0;
int velocidadeEsq = 0;
int rpm = 0;
float velocidademedida = 0.0f;

// velocidade
float correcaoDir = 0.91f;
float angulo = 0.0;
float modulo = 0.0f;
int valorFinalModulo = 0;
float correcao = 0.0;
float multiDir = 0.0;
float multiEsq = 0.0;
String comando = "";

// MOTOR
AF_DCMotor motorEsquerda(1);
AF_DCMotor motorDireita(2);

// ENCODER
RotaryEncoder encoderEsquerda(18, 19);
RotaryEncoder encoderDireita(20, 21);
int posicaoEsquerdaAnterior = 0;
int posicaoDireitaAnterior = 0;

// Garra 
int pinoDoServo = 10;
Servo servo;
int estadoBotao1 = 0;

// led 
int estadoBotao2 = 0;
int led = 39;




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
  // Serial
  Serial.begin(9600);
  Serial.println("entrei");
  Serial3.begin(9600);

  // VCC E GROUND
  pinMode(16, OUTPUT);
  pinMode(17, OUTPUT);

  digitalWrite(16, LOW);
  digitalWrite(17, HIGH);

  // MOTOR
  motorEsquerda.setSpeed(255);
  motorDireita.setSpeed(255*correcaoDir);
  //motorEsquerda.run(FORWARD);
  //motorDireita.run(FORWARD);


  // Garra
  servo.attach(10);
  servo.write(0);

  // LED
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);

  // ENCODER
  int origem1 = digitalPinToInterrupt(20);
  attachInterrupt(origem1, tickDoEncoderDireita, CHANGE);
  int origem2 = digitalPinToInterrupt(21);
  attachInterrupt(origem2, tickDoEncoderDireita, CHANGE);
  int origem3 = digitalPinToInterrupt(18);
  attachInterrupt(origem3, tickDoEncoderEsquerda, CHANGE);
  int origem4 = digitalPinToInterrupt(19);
  attachInterrupt(origem4, tickDoEncoderEsquerda, CHANGE);
}


void tickDoEncoderEsquerda() {
  encoderEsquerda.tick();
}

void tickDoEncoderDireita() {
  encoderDireita.tick();
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  // MOTOR + VELOCIDADE
  if (Serial3.available() > 0) {
    String texto = Serial3.readStringUntil('\n');
    texto.trim();  // remove quebra de linha
    if (texto.startsWith("V")){
      int pos = texto.indexOf(':');
      int posVirgula = texto.indexOf(',');
      modulo = (texto.substring(pos + 1, posVirgula).toFloat());
      angulo = (texto.substring(posVirgula + 1).toFloat());
    }
    if (texto.startsWith("G")){
      estadoBotao1= (texto.substring(1).toFloat());
      if (estadoBotao1 == 0){
        servo.write(0);
      }
      else if (estadoBotao1 == 1){
        servo.write(25);
      }
    }
    if (texto.startsWith("L")){
      estadoBotao2= (texto.substring(1).toFloat());
      if (estadoBotao2 == 0){
        digitalWrite(led, LOW);
      }
      else if (estadoBotao2 == 1){
        digitalWrite(led, HIGH);
      }
    } 
    if (angulo > 0.0 && angulo < 90.0){
      multiEsq = (angulo / 90.0);
      multiDir = 1.0;
    } 
    else if (angulo > 90.0 && angulo < 180.0){
      multiEsq = 1.0;
      multiDir = 1.0 - ((angulo-90) / 90.0);
    }
    else if (angulo > 180.0 && angulo < 270.0){
      multiEsq = 1.0;
      multiDir = ((angulo-180) / 90.0);
    }
    else if (angulo > 270.0 && angulo < 360.0){
      multiEsq = 1-((angulo-270)/90.0);
      multiDir = 1.0;
    }
    else{
      multiDir = 1.0;
      multiEsq = 1.0;
    }

    valorFinalModulo = map(modulo, 0, 724, 0, 255);
    motorEsquerda.setSpeed(valorFinalModulo * multiEsq);
    motorDireita.setSpeed(valorFinalModulo * multiDir * correcaoDir);
    //Serial.print("velocidade Direita:");
   //Serial.print(valorFinalModulo * multiDir);
    //Serial.print("velocidade Esquerda:");
   //Serial.print(valorFinalModulo * multiEsq);

    //Serial.println(valorFinalModulo);
    //Serial.println("Modulo: " + String(modulo) + " Angulo: " + String(angulo));
    if (angulo > 0.00f && angulo < 180.00f) {
      comando = "Frente";
    }
    if (angulo > 180.00f && angulo < 360.00f) {
      comando = "Tras";
    }
    if (angulo == 180.00){
      comando = "Esquerda";
    } 
    if (angulo == 0.0 || angulo == 360.0) {
      comando = "Direita";
    }


      //rodar direcao
    /*if (texto.startsWith("DireitaF")) {
      int pos = texto.indexOf(':');
      velocidadeDir = texto.substring(pos + 1).toInt();
      Serial.println(velocidadeDir);
      motorDireita.setSpeed(velocidadeDir);
      motorDireita.run(FORWARD);
    }
    if (texto.startsWith("DireitaT")) {
      int pos = texto.indexOf(':');
      velocidadeDir = texto.substring(pos + 1).toInt();
      Serial.println(velocidadeDir);
      motorDireita.setSpeed(velocidadeDir);
      motorDireita.run(BACKWARD);
    }
    if (texto.startsWith("EsquerdaF")) {
      int pos = texto.indexOf(':');
      velocidadeEsq = texto.substring(pos + 1).toInt();
      //Serial.println(velocidadeEsq);
      motorEsquerda.setSpeed(velocidadeEsq);
      motorEsquerda.run(FORWARD);
    }
    if (texto.startsWith("EsquerdaT")) {
      int pos = texto.indexOf(':');
      velocidadeEsq = texto.substring(pos + 1).toInt();
      //Serial.println(velocidadeEsq);
      motorEsquerda.setSpeed(velocidadeEsq);
      motorEsquerda.run(BACKWARD);
    }
    Serial.println(comando);*/
    if (comando == "Frente") {
      motorEsquerda.run(FORWARD);
      motorDireita.run(FORWARD);
    }
    if (comando == "Tras") {
      motorEsquerda.run(BACKWARD);
      motorDireita.run(BACKWARD);
    }

    if (comando == "Esquerda") {
      motorEsquerda.run(FORWARD);
      motorDireita.run(BACKWARD);
    }
    if (comando == "Direita") {
      motorEsquerda.run(BACKWARD);
      motorDireita.run(FORWARD);
    }
    if (texto == "Parar") {
      motorEsquerda.run(RELEASE);
      motorDireita.run(RELEASE);
    }
    comando = "";

    /*if(botaoLED == true){
      digitalWrite(led, LOW);
    }
    else {
      digitalWrite(led, HIGH);
    }*/
  }


  // ENCODER
  int posicaoEsquerda = encoderEsquerda.getPosition();
  if (posicaoEsquerda != posicaoEsquerdaAnterior) {
    //Serial.println("posição esquerda: " + String(posicaoEsquerda));
  }
  posicaoEsquerdaAnterior = posicaoEsquerda;

  int posicaoDireita = encoderDireita.getPosition();
  if (posicaoDireita != posicaoDireitaAnterior) {
    //Serial.println("posição direita: " + String(posicaoDireita));
  }
  posicaoDireitaAnterior = posicaoDireita;

  // registro velocidade
  unsigned long instanteAtual = millis();
  if (instanteAtual > instanteContagem + 1000) {
    pulsosNoSegundoEsq = abs(posicaoEsquerda - pulsoInicialEsq);
    pulsosNoSegundoDir = abs(posicaoDireita - pulsoInicialDir);
    if (pulsosNoSegundoEsq >= pulsosNoSegundoDir) {
      rpm = (pulsosNoSegundoEsq * 60) / TicksPorRotacao;
    }
    if (pulsosNoSegundoDir > pulsosNoSegundoEsq) {
      rpm = (pulsosNoSegundoDir * 60) / TicksPorRotacao;
    }
    velocidademedida = ((rpm * PI * 56) / 60000) * 3.6f;
    pulsoInicialEsq = posicaoEsquerda;
    pulsoInicialDir = posicaoDireita;
    Serial3.print("v");
    Serial3.println(velocidademedida);
    instanteContagem = instanteAtual;
  }


  //teste dos encoders
  /*motorDireita.setSpeed(255*correcaoDir);
  motorEsquerda.setSpeed(255);
  motorEsquerda.run(FORWARD);
  motorDireita.run(FORWARD);
  if (instanteAtual > instanteContagemTeste + 1000) {
    Serial.println(posicaoEsquerda-posicaoDireita);
    instanteContagemTeste = instanteAtual;
  }*/
}
