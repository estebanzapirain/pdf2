/*
pdf1.ino
Calcula la pdf de una señal entrante en el ADC y la muestra por puerto serie


*/


#include "pico/stdlib.h"
//#include "MCP_DAC.h"

repeating_timer_t timer;

//ADC0 -> GPIO26
#define ADC_PIN 26

//Frecuencia de muestreo (maximo 500ksps)
const int fclk = 500000;

//Cantidad de valores posibles = 2^10= 1024
const int n_valores = 256;

//Cantidad de muestras
const int n_muestras = 20000;

const int dacxPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // D0-
const int dacyPins[8] = {10,11,12,13,14,15,16,17}; // D0-
uint16_t value;

//Definición de buffers
uint16_t muestras[n_muestras];
uint16_t pdf[n_valores];

int i = 0;

bool mostrar_pdf = false;
bool muestrear = true;

//MCP4922 MCP;

void setup() {
  delay(1000);  //retraso para que no interfiera el DACy con el USB (pin 15)
   for (int i = 0; i < 8; i++) {
    pinMode(dacxPins[i], OUTPUT);
    pinMode(dacyPins[i], OUTPUT);
  }
  add_repeating_timer_us(-1000000 / fclk, timer_callback, NULL, &timer);
  //SPI.begin();
  Serial.begin(250000);
  analogReadResolution(8);

  //MCP.begin(17);
}

bool timer_callback(repeating_timer_t *rt) {
  if (muestrear) {
    uint16_t v = analogRead(ADC_PIN);
    muestras[i++] = v;
    pdf[v]++;  // Incrementa PDF en tiempo real
    if (i == n_muestras) {
      mostrar_pdf = true;
      muestrear = false;
      i = 0;
    }
  }
  return true;
}

void escribirDACx(byte valor) {
  for (int i = 0; i < 8; i++) {
    // Extrae cada bit y lo escribe en el pin correspondiente
    digitalWrite(dacxPins[i], (valor >> i) & 1);
  }
}
void escribirDACy(byte valor) {
  for (int i = 0; i < 8; i++) {
    // Extrae cada bit y lo escribe en el pin correspondiente
    digitalWrite(dacyPins[i], (valor >> i) & 1);
  }
}
void loop() {
  if (mostrar_pdf) {
    mostrar_pdf = false;
    // Muestra la PDF
    for (int j = 0; j < n_valores; j++) {
      //Serial.println(pdf[j]);
      escribirDACx(j);
      delay(2);
      escribirDACy(pdf[j]);
      //escribirDACy(j);
      delay(2);
    }
    // Reset para la próxima medición
    memset(pdf, 0, sizeof(pdf));
    muestrear = true;
  }
}
