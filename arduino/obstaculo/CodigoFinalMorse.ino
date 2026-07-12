#include <ShiftDisplay.h>
int index[] = {3,2,1,0};
ShiftDisplay display(6, 7, 5, COMMON_ANODE, 4, true, index);
char sorteio[3];
int sensor(A1);
unsigned long tempoAnt;
bool haLuz = false;
bool haviaLuz = false;
int piscada;
int contador = 0;
int i = 0;
int ledVd = 13;
int ledVerm = 12;
bool estadoVerde = false;
bool estadoVermelho = true;
char senha[3];
int letras[4][4] = { { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 }, { -1, -1, -1, -1 } };
int tabela[18][4] = { { 0, 1, -1, -1 }, { 1, 0, 0, 0 }, { 1, 0, 1, 0 }, { 1, 0, 0, -1 }, { 0, -1, -1, -1 }, { 0, 0, 1, 0 }, 
                      { 1, 1, 0, -1 }, { 0, 0, 0, 0 }, { 0, 0, -1, -1 }, { 0, 1, 1, 1 }, { 0, 1, 0, 0 }, { 1, 1, 1, -1 }, 
                      { 0, 1, 1, 0 }, { 0, 1, 0, -1 }, { 0, 0, 0, -1 }, { 1, -1, -1, -1 }, { 0, 0, 1, -1 }, { 1, 0, 1, 1 }};
char caracteres[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'L', 'O', 'P', 'R', 'S', 'T', 'U', 'Y'};

void sortear() {
  randomSeed(analogRead(A0));
  for (int i = 0; i < 2; i++) {
    int n = random(0, 18);
    sorteio[i] = caracteres[n];
  }
  display.set(sorteio);
}

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT);
  pinMode(ledVd, OUTPUT);
  pinMode(ledVerm, OUTPUT);
  sortear();
  for (int j = 0; j < 2; j++) {
    Serial.println(sorteio[j]);
  }
}

void printPalavra() {
  for (int n = 0; n <= 3; n++) {
    for (int j = 0; j <= 3; j++) {
      if (letras[n][j] == 1) Serial.print("-");
      else if (letras[n][j] == 0) Serial.print(".");
    }
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {
  if (estadoVerde) digitalWrite(ledVd, HIGH);
  else digitalWrite(ledVd, LOW);
  if (estadoVermelho) digitalWrite(ledVerm, HIGH);
  else digitalWrite(ledVerm, LOW);
  if (analogRead(sensor) > 250) haLuz = false;
  else haLuz = true;
  if (i <= 1) {
    if (haLuz != haviaLuz && haLuz) {
      if (i != 0 || contador != 0) {
        if (millis() - tempoAnt >= 2000) {
          i++;
          contador = 0;
        }
      }
      tempoAnt = millis();
    } else if (haLuz != haviaLuz && haLuz == false) {
      if (millis() - tempoAnt <= 300) {
        piscada = 0;
        letras[i][contador] = piscada;
        contador++;
      } else {
        piscada = 1;
        letras[i][contador] = piscada;
        contador++;
      }
      if (contador > 3) {
        i++;
        contador = 0;
      }
      tempoAnt = millis();
      printPalavra();
    }
    haviaLuz = haLuz;
  }
  else if (i == 2) {
    for (int k = 0; k <= 3; k++) {
      for (int t = 0; t <= 17; t++) {
        if (letras[k][0] == tabela[t][0] && letras[k][1] == tabela[t][1] && letras[k][2] == tabela[t][2] && letras[k][3] == tabela[t][3]) {
          senha[k] = caracteres[t];
        }
      }
    }
    for (int d = 0; d < 2; d++) {
      Serial.print(senha[d]);
    }
    i = 3;
    if (sorteio[0] == senha[0] && sorteio[1] == senha[1]) {
      estadoVermelho = false;
      estadoVerde = true;
    }
  }
  display.update();
}
