#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"
#include <EEPROM.h>

#define MAXSIZEBUFF 200
#define R 8.314
#define F 96485
#define N 1 // Número de elétrons transferidos na reação do H+ (hidrogênio)

LiquidCrystal_I2C lcd(0x27, 16, 2);

float valor_calibracao = 21.34; // Fator de calibração

int contagem = 0;           // Variável de contagem
float soma_tensao = 0;      // Variável para soma de tensão
float media = 0;            // Variável que calcula a media
float entrada_A0;           // Variável de leitura do pino A0
float tensao;               // Variável para conversão em tensão
unsigned long tempo;        // Float tempo
float valor_pH;             // PH
float Temperatura;          // Temperatura
float media_analogica = 0;

int SO = 12;
int CS = 10;
int CLK = 13;
int PINPH = A0;



char buffer[MAXSIZEBUFF]; // buffer de dados

MAX6675 sensor(CLK, CS, SO);

void Start_PH() {
  pinMode(PINPH, INPUT);
}

void Start_Serial() {
  Serial.begin(115200);
}

void Start_LCD() {
  lcd.init();                   // Inicializa o display
  lcd.begin(16, 2);             // Inicializa todos os caracteres
  lcd.backlight();              // Inicializa o backlight
  lcd.setCursor(0, 0);          // Posiciona o cursor na posição
  lcd.print("PH Monitor");      // Escreve no monitor serial
  lcd.setCursor(0, 1);          // Posiciona o cursor na posição
  lcd.print("Leitura de pH");   // Escreve no monitor serial
}

// Função para armazenar um valor float na EEPROM e atribuí-lo à variável valor_calibracao
void Start_EEPROM() {
  // Lê o valor da EEPROM e o atribui à variável valor_calibracao
  EEPROM.get(0, valor_calibracao);

  Serial.print("Valor de calibração armazenado: ");
  Serial.println(valor_calibracao);
}

void Start_Ambiente() {
  memset(buffer, '\0', MAXSIZEBUFF); // Inicializa buffer de dados
  // Coleta valor de calibração
  Start_EEPROM();
}

void setup() {
  Start_Serial();
  Start_Ambiente();
  Start_PH();
  Start_LCD();
  delay(2000);
  lcd.clear(); // Limpa o conteúdo do display
}

void Grava_ValorCalibracao(float valor) {
  // Armazena o valor float na EEPROM
  EEPROM.put(0, valor);
}

void Le_Temperatura() {
  Temperatura = sensor.readCelsius();
  Serial.print("Graus C = ");
  Serial.println(Temperatura);
}

float calcularInclinacao(float temperaturaCelsius) {
    float temperaturaKelvin = temperaturaCelsius + 273.15;
    return (R * temperaturaKelvin) / (F * N); // Calcula a inclinação para pH
} 

void Le_PH() {
    soma_tensao = 0;  // Inicia soma_tensão em 0
    contagem = 0;     // Inicia a contagem em 0

    while (contagem < 10) {
        entrada_A0 = analogRead(PINPH);               // Lê a entrada analógica
        media_analogica = (entrada_A0 + media_analogica) / 2;

        tensao = (media_analogica * 5.0) / 1024.0;    // Converte em tensão, o valor lido
        soma_tensao += tensao;                        // Soma a tensão anterior com a atual
        contagem++;                                   // Soma 1 à variável de contagem

        delay(100);                                   // Aguarda para próxima leitura
    }
    media = soma_tensao / 10;                         // Calcula a média das leituras

    float inclinacao = calcularInclinacao(Temperatura);
    valor_pH = (media - 7) / inclinacao; // Calcula valor de pH usando a inclinação corrigida
}

void Display() {
  lcd.setCursor(0, 0);            // Posiciona o cursor no display
  lcd.print("Valor pH: ");        // Escreve no display
  lcd.setCursor(11, 0);           // Posiciona o cursor no display
  lcd.print(valor_pH, 1);         // Escreve o pH com uma casa decimal
  lcd.setCursor(0, 1);            // Posiciona o cursor no display
  lcd.print("Temp: ");            // Escreve no display
  lcd.setCursor(11, 1);           // Posiciona o cursor no display
  lcd.print(Temperatura, 1);      // Escreve o pH com uma casa decimal
}

void Le_Serial() {
  static String inputString = "";         // String para armazenar os dados de entrada
  char carac;

  while (Serial.available() > 0) {       // Se houver dados disponíveis na serial
    carac = (char)Serial.read();         // Lê o próximo caractere

    if (carac != '\n') {                 // Se não for o fim da linha
      inputString += carac;              // Adiciona o caractere à string de entrada
    } else {                             // Se for o fim da linha
      if (inputString.startsWith("calibrar:")) { // Verifica se a string começa com "calibrar:"
        String valorString = inputString.substring(9); // Extrai o valor após "calibrar:"
        float valor = valorString.toFloat();           // Converte a string para float
        Grava_ValorCalibracao(valor);                  // Chama a função para gravar o valor
        Serial.print("Novo valor de calibração: ");
        Serial.println(valor);
      }
      inputString = "";                  // Limpa a string de entrada para a próxima leitura
    }
  }
}

void loop() {
  Le_PH();
  Le_Temperatura();
  Le_Serial();
  Display();

  // Aguarda para próxima leitura
  delay(1000);
}
