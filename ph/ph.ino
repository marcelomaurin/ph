#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

float valor_calibracao = 21.34;   // Fator de calibração

int contagem = 0;           // Variável de contagem
float soma_tensao = 0;      // Variável para soma de tensão
float media = 0;            // Variável que calcula a media
float entrada_A0;           // Variável de leitura do pino A0
float tensao;               // Variável para conversão em tensão
unsigned long tempo;        // Float tempo

void setup() {
  lcd.init();                   // Inicializa o display
  lcd.begin(16, 2);             // Inicializa todos os caracteres
  lcd.backlight();              // Inicializa o backlight
  lcd.setCursor(0, 0);          // Posiciona o cursor na posição
  lcd.print("USINAINFO");       // Escreve no monitor serial
  lcd.setCursor(0, 1);          // Posiciona o cursor na posição
  lcd.print("Leitura de pH");   // Escreve no monitor serial
  delay(2000);
  lcd.clear();                  // Limpa o conteúdo do display
}

void loop() {
  soma_tensao = 0;   // Inicia soma_tensão em 0
  contagem = 0;      // Inicia a contagem em 0

  while (contagem < 10) {                   // Executa enquanto contagem menor que 10
    tempo = millis();                       // Define o tempo em microssegundos
    entrada_A0 = analogRead(A0);            // Lê a entrada analógica
    tensao = (entrada_A0 * 5.0) / 1024.0;   // Converte em tensão, o valor lido
    soma_tensao = (soma_tensao + tensao);   // Soma a tensão anterior com a atual
    contagem++;                             // Soma 1 à variável de contagem
    delay(100);                             // Aguarda para próxima leitura
  }

  media = soma_tensao / 10;                 // Calcula a média das leituras

  float valor_pH = -5.70 * media + valor_calibracao;    // Calcula valor de pH

  lcd.setCursor(0, 0);            // Posiciona o cursor no display
  lcd.print("Valor pH: ");        // Escreve no display
  lcd.setCursor(11, 0);           // Posiciona o cursor no display
  lcd.print(valor_pH, 1);         // Escreve o pH com uma casa decimal
  delay(1000);                    // Aguarda para próxima leitura
}