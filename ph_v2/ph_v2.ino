/*
  Medidor de pH - Firmware V2
  Placa: Arduino Nano classico (ATmega328P, 5 V)

  Display LCD I2C 16x2:
    SDA -> A4
    SCL -> A5

  Modulo de pH:
    PO/AO -> A0

  MAX6675:
    SO/DO -> D12
    CS    -> D10
    SCK   -> D13

  A sonda de pH deve ser conectada a um modulo condicionador.
  Nunca conecte a sonda diretamente ao pino A0.
*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "max6675.h"
#include <math.h>

// ------------------------- Hardware -------------------------
const byte PIN_PH = A0;
const byte PIN_MAX6675_SO = 12;
const byte PIN_MAX6675_CS = 10;
const byte PIN_MAX6675_CLK = 13;

LiquidCrystal_I2C lcd(0x27, 16, 2);
MAX6675 sensorTemperatura(PIN_MAX6675_CLK,
                          PIN_MAX6675_CS,
                          PIN_MAX6675_SO);

// ---------------------- Configuracao ------------------------
const unsigned long INTERVALO_LEITURA_MS = 1000UL;
const byte QUANTIDADE_AMOSTRAS = 10;
const unsigned int INTERVALO_AMOSTRA_MS = 20;

// Arduino Nano classico: ADC de 10 bits e referencia de 5 V.
// Para melhorar a exatidao, meca os 5 V reais e ajuste este valor.
const float TENSAO_REFERENCIA_ADC = 5.00;
const float ADC_MAXIMO = 1023.0;

/*
  Coeficientes iniciais do modulo condicionador:

    pH = INCLINACAO_PH * tensao + OFFSET_PH

  Estes valores permitem o teste inicial, mas devem ser ajustados com
  solucoes tampao, preferencialmente pH 7,00 e pH 4,00 ou pH 10,00.
*/
const float INCLINACAO_PH = -5.70;
const float OFFSET_PH = 21.34;

unsigned long ultimaLeitura = 0;

// ----------------------- Inicializacao ----------------------
void mostrarTelaInicial(const __FlashStringHelper *linha1,
                        const __FlashStringHelper *linha2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linha1);
  lcd.setCursor(0, 1);
  lcd.print(linha2);
}

void iniciarDisplay() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  mostrarTelaInicial(F("Monitor de pH"), F("Inicializando..."));
  delay(1200);

  mostrarTelaInicial(F("Display: OK"), F("Sensor temp..."));
  delay(800);

  // O MAX6675 necessita de um pequeno tempo para a primeira conversao.
  float temperaturaTeste = sensorTemperatura.readCelsius();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Temperatura:"));
  lcd.setCursor(0, 1);

  if (isnan(temperaturaTeste)) {
    lcd.print(F("Sensor ausente"));
  } else {
    lcd.print(F("Sensor OK"));
  }

  delay(1200);

  mostrarTelaInicial(F("Sistema pronto"), F("Lendo sensores"));
  delay(1000);
  lcd.clear();
}

void setup() {
  pinMode(PIN_PH, INPUT);
  Serial.begin(115200);

  iniciarDisplay();

  Serial.println(F("Monitor de pH V2 iniciado"));
  Serial.println(F("Placa: Arduino Nano ATmega328P"));
  Serial.println(F("PH=A0, MAX6675: SO=12 CS=10 CLK=13"));
}

// ------------------------- Leituras -------------------------
float lerTensaoPH() {
  unsigned long somaADC = 0;

  for (byte i = 0; i < QUANTIDADE_AMOSTRAS; i++) {
    somaADC += analogRead(PIN_PH);
    delay(INTERVALO_AMOSTRA_MS);
  }

  float mediaADC = somaADC / (float)QUANTIDADE_AMOSTRAS;
  return mediaADC * TENSAO_REFERENCIA_ADC / ADC_MAXIMO;
}

float calcularPH(float tensao) {
  float ph = INCLINACAO_PH * tensao + OFFSET_PH;

  // Evita apresentar valores fora da escala convencional do prototipo.
  if (ph < 0.0) {
    ph = 0.0;
  } else if (ph > 14.0) {
    ph = 14.0;
  }

  return ph;
}

// ------------------------- Display --------------------------
void limparLinha(byte linha) {
  lcd.setCursor(0, linha);
  lcd.print(F("                "));
}

void mostrarErroTemperatura(float ph) {
  char textoPH[7];
  dtostrf(ph, 4, 1, textoPH);

  limparLinha(0);
  lcd.setCursor(0, 0);
  lcd.print(F("pH: "));
  lcd.print(textoPH);

  limparLinha(1);
  lcd.setCursor(0, 1);
  lcd.print(F("Temp: ERRO"));
}

void mostrarMedicoes(float ph, float temperatura) {
  char textoPH[7];
  char textoTemperatura[8];

  dtostrf(ph, 4, 1, textoPH);
  dtostrf(temperatura, 5, 1, textoTemperatura);

  limparLinha(0);
  lcd.setCursor(0, 0);
  lcd.print(F("pH: "));
  lcd.print(textoPH);

  limparLinha(1);
  lcd.setCursor(0, 1);
  lcd.print(F("Temp: "));
  lcd.print(textoTemperatura);
  lcd.write((byte)223); // Simbolo de grau do LCD HD44780
  lcd.print(F("C"));
}

void enviarSerial(float tensao, float ph, float temperatura) {
  Serial.print(F("Tensao pH: "));
  Serial.print(tensao, 3);
  Serial.print(F(" V | pH: "));
  Serial.print(ph, 2);
  Serial.print(F(" | Temperatura: "));

  if (isnan(temperatura)) {
    Serial.println(F("ERRO"));
  } else {
    Serial.print(temperatura, 2);
    Serial.println(F(" C"));
  }
}

// --------------------------- Ciclo --------------------------
void loop() {
  unsigned long agora = millis();

  if (agora - ultimaLeitura < INTERVALO_LEITURA_MS) {
    return;
  }

  ultimaLeitura = agora;

  // A temperatura e lida antes do pH para evitar o valor atrasado da V1.
  float temperatura = sensorTemperatura.readCelsius();
  float tensaoPH = lerTensaoPH();
  float ph = calcularPH(tensaoPH);

  if (isnan(temperatura)) {
    mostrarErroTemperatura(ph);
  } else {
    mostrarMedicoes(ph, temperatura);
  }

  enviarSerial(tensaoPH, ph, temperatura);
}

