#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#include <GFButton.h>
MCUFRIEND_kbv tela;

// Cores Custom
#define COR_VERDE_CLARO 0x9E66

// Velocidade Maxima
const int VEL_MAX = 15;

// Joystick
const int pinoX = A10;       // VRx
const int pinoY = A11;       // VRy
int pinoBotaoJoystick = 40;  // SW

// Botao Garra
GFButton botao(30);
GFButton botao2(26);

float velocidade = 10.0;
bool garraAberta = false;
int led = 0;

float velocidadeMedida = 0;

unsigned long tempoAtual = 0;
unsigned long tempoAnterior = 0;
const long intervalo = 1000;
int segundosCorrida = 0;
int minutosCorrida = 0;
char tempoDisplay[6];

const int xCentro = 80;
const int yCentro = 140;
const int raio = 70;
const int raioInt = 57;
int ultimoAngulo = 180;

void setup() {
  Serial.begin(9600);

  // Transmissao para o carrinho
  Serial1.begin(9600);

  // GFButton
  botao.setPressHandler(funcaoBotao);
  botao2.setPressHandler(funcaoBotao2Apertar);
  botao2.setReleaseHandler(funcaoBotao2Soltar);

  // Configuração dos pinos com resistor interno ativo
  pinMode(30, INPUT_PULLUP);  // Botao Garra
  pinMode(40, INPUT_PULLUP);  // Botao Joystick
  pinMode(26, INPUT_PULLUP);

  tela.begin(tela.readID());
  tela.fillScreen(TFT_BLACK);
  tela.setRotation(1);

  // Titulo Velocimetro
  tela.setTextSize(2);
  tela.setTextColor(TFT_WHITE);
  tela.setCursor(10, 20);
  tela.print("VELOCIMETRO");

  // Unidades de medida fixas
  tela.setTextSize(2);
  tela.setTextColor(TFT_YELLOW);
  tela.setCursor(265, 35);
  tela.print("km/h");

  // Bussola Direcional Estatica

  // Desenho do Arco do Velocímetro
  for (int i = 180; i >= 0; i--) {
    uint16_t cor;

    if (i > 145) cor = COR_VERDE_CLARO;
    else if (i > 115) cor = TFT_GREEN;
    else if (i > 75) cor = TFT_YELLOW;
    else if (i > 35) cor = TFT_ORANGE;
    else cor = TFT_RED;

    float rad = i * DEG_TO_RAD;

    int xExt = xCentro + raio * cos(rad);
    int yExt = yCentro - raio * sin(rad);
    int xInt = xCentro + raioInt * cos(rad);
    int yInt = yCentro - raioInt * sin(rad);

    tela.drawLine(xInt, yInt, xExt, yExt, cor);
  }
}

void loop() {
  botao.process();
  botao2.process();
  // Lê os valores analógicos do Joystick
  int valorX = analogRead(pinoX);
  int valorY = analogRead(pinoY);
  int xCentrado = valorX - 512;
  int yCentrado = valorY - 512;
  if (abs(xCentrado) < 60) xCentrado = 0;
  if (abs(yCentrado) < 60) yCentrado = 0;
  int anguloCarrinho = 0;  // Variável para armazenar o ângulo
  if (xCentrado != 0 || yCentrado != 0) {
    float radianos = atan2(yCentrado, xCentrado);
    anguloCarrinho = radianos * 180.0 / PI;
    if (anguloCarrinho < 0) {
      anguloCarrinho += 360;  // Normaliza de 0 a 360 graus
    }
  }
  float velMod = mod(yCentrado, xCentrado);
  // Serial.print("velMod: ");
  // Serial.println(velMod);
  float velMapeada = map(velMod, 0, 724, 0, VEL_MAX);
  velocidade = velMapeada / 10.0;

  // Cronômetro
  tempoAtual = millis();
  if (tempoAtual - tempoAnterior >= intervalo) {
    atualizarPonteiro(velocidadeMedida*10);
    tempoAnterior = tempoAtual;
    segundosCorrida++;
    // Cronometro
    if (segundosCorrida >= 60) {
      segundosCorrida = 0;
      minutosCorrida++;
    }
    tela.setTextSize(4);
    tela.setTextColor(TFT_BLACK);
    tela.setCursor(180, 120);
    tela.print(tempoDisplay);
    sprintf(tempoDisplay, "%02d:%02d", minutosCorrida, segundosCorrida);
    tela.setTextColor(TFT_WHITE);
    tela.setCursor(180, 120);
    tela.print(tempoDisplay);

    // Dados de Debug na Tela
    tela.setCursor(10, 220);
    tela.setTextSize(2);
    tela.setTextColor(TFT_WHITE, TFT_BLACK);
    tela.print("Debug: ");
    tela.print(" X: ");
    tela.print(valorX);
    tela.print(" Y: ");
    tela.print(valorY);
    tela.print("      ");

    // Sentido Frente / Tras (CIMA / BAIXO)

    tela.setCursor(190, 170);
    tela.setTextSize(3);

    // yCentrado (Eixo Y para acelerar/frear)
    if (yCentrado == 0) {
      tela.setTextColor(TFT_WHITE, TFT_BLACK);
      tela.print("Parado");
    } else if (yCentrado > 0) {
      tela.setTextColor(TFT_GREEN, TFT_BLACK);
      tela.print("Frente");
    } else {
      tela.setTextColor(TFT_RED, TFT_BLACK);
      tela.print("Tras  ");
    }

    // Bussola Direcional Dinamica (ESQUERDA / DIREITA)

    tela.fillRect(10, 180, 140, 10, TFT_BLACK);

    // Desenha a marcação base (cruzamento)
    tela.drawFastHLine(10, 190, 140, TFT_WHITE);
    tela.drawFastVLine(80, 170, 20, TFT_WHITE);

    // xCentrado (Eixo X para os lados)
    if (xCentrado != 0) {

      int larguraBarra = map(abs(xCentrado), 0, 512, 0, 60);

      if (xCentrado > 0) {
        // Movendo para a DIREITA
        tela.fillRect(80, 180, larguraBarra, 10, TFT_WHITE);
      } else {
        // Movendo para a ESQUERDA
        tela.fillRect(80 - larguraBarra, 180, larguraBarra, 10, TFT_WHITE);
      }
    }
    // Velocidade Digital na Tela
    tela.setCursor(170, 20);
    tela.setTextColor(TFT_YELLOW, TFT_BLACK);
    tela.setTextSize(4);
    if (velocidade < 0.5) {
      tela.print(0.0, 1);
    } else {
      tela.print(velocidade, 1);
    }
    tela.print(" ");
  }

  // Transmissão Serial para o Arduino do Carrinho
  Serial1.print("V:");
  Serial1.print(velMod);
  Serial1.print(",");
  Serial1.print(anguloCarrinho);
  Serial1.print("\r\n");

  // Recebimento da Velocidade
  if (Serial1.available() > 0) {
    String texto = Serial1.readStringUntil('\n');
    texto.trim();
    if (texto.startsWith("v")) { 
      velocidadeMedida = texto.substring(1).toFloat();
      Serial.println(velocidadeMedida);
    }
  }

  //delay(300);
}

void atualizarPonteiro(int valor) {
  if (valor > VEL_MAX) {
    valor = VEL_MAX;
  }

  int angulo = map(valor, 0, VEL_MAX, 180, 0);

  // Apaga ponteiro anterior
  float radAntigo = ultimoAngulo * DEG_TO_RAD;
  int xAntigo = xCentro + (raioInt - 4) * cos(radAntigo);
  int yAntigo = yCentro - (raioInt - 4) * sin(radAntigo);
  tela.drawLine(xCentro, yCentro, xAntigo, yAntigo, TFT_BLACK);

  // Linha do ponteiro
  float radNovo = angulo * DEG_TO_RAD;
  int xNovo = xCentro + (raioInt - 4) * cos(radNovo);
  int yNovo = yCentro - (raioInt - 4) * sin(radNovo);
  tela.drawLine(xCentro, yCentro, xNovo, yNovo, TFT_WHITE);

  // Pino Central
  tela.fillCircle(xCentro, yCentro, 8, TFT_WHITE);
  tela.drawCircle(xCentro, yCentro, 8, TFT_BLACK);

  ultimoAngulo = angulo;
}

float mod(int X, int Y) {
  // Serial.print("Entrada: ");
  // Serial.print(X);
  // Serial.print(" | ");
  // Serial.println(Y);
  float fx = X;
  float fy = Y;
  return sqrt(fx * fx + fy * fy);
}

int funcaoBotao() {
  garraAberta = !garraAberta;
  Serial1.print("G");
  Serial1.print(garraAberta ? 1 : 0);
  Serial1.print("\r\n");
  // if (garraAberta == true) {
  //   Serial.print("Aberta");
  // } else {
  //   Serial.print("Fechada");
  // }

  // Status Visual da Garra na Tela
  tela.setCursor(180, 80);
  if (garraAberta == true) {
    tela.setTextSize(3);
    tela.setTextColor(TFT_GREEN, TFT_BLACK);
    tela.print("\\/");
    tela.setTextSize(2);
    tela.print(" ABERTA ");
  } else {
    tela.setTextSize(3);
    tela.setTextColor(TFT_RED, TFT_BLACK);
    tela.print("/\\");
    tela.setTextSize(2);
    tela.print(" FECHADA ");
  }
}
void funcaoBotao2Apertar() {
  Serial1.println("L1");
}

void funcaoBotao2Soltar() {
  Serial1.println("L0");
}