/*
pdf1.ino
Calcula la pdf de una señal entrante en el ADC
Tiene conectados 2 DAC R-2R de 8 bits
Saca la rampa de barrido por DACx
PDF por DACy

Rampa escalonada
PDF con escalones, altura de escalón variable
Ancho de ventana variable
*/


#include "pico/stdlib.h"
#include "comandos.h"

repeating_timer_t timer;

//ADC0 -> GPIO26
#define ADC_PIN 26

// Frecuencia de muestreo (maximo 500ksps)
const int fclk = 500000;

// Cantidad de valores posibles = resolución del ADC
const int n_valores = 256;

// Contador de muestras
uint32_t muestra = 0;

// Pines asociados a los DACs
const int dacxPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; 
const int dacyPins[8] = {10,11,12,13,14,15,16,17}; 

// Definición de buffers
uint8_t pdf[n_valores];

// Lógica de estados
bool mostrar_pdf = false;
bool muestrear = true;

// Parámetros
volatile uint8_t altura_escalon = 10;
volatile uint8_t ancho_ventana = 10;
volatile uint8_t n_escalones = 256; //resolución del ADC = resolución del DAC
volatile uint32_t n_muestras = 5000;

// Cadena de comando
String comando = "";

void setup() {
  delay(1000);  //retraso para que no interfiera el DACy con el USB (pin 15)
   for (int i = 0; i < 8; i++) {
    pinMode(dacxPins[i], OUTPUT);
    pinMode(dacyPins[i], OUTPUT);
  }
  add_repeating_timer_us(-1000000 / fclk, timer_callback, NULL, &timer);
  
  Serial.begin(115200);
  comando.reserve(256);
  analogReadResolution(8);

}

bool timer_callback(repeating_timer_t *rt) {
  if (muestrear) {
    
    uint8_t v = analogRead(ADC_PIN); //muestrea

    int piso_escalon = 0; //comienza a recorrer los escalones buscando a cuál corresponde la muestra
    for (int escalon = 0;escalon<n_valores;escalon++){ //recorre todos los escalones
      if ((v>=piso_escalon) && (v<piso_escalon + ancho_ventana)){ //si la muestra entra en la ventana
          pdf[escalon]++; //incremento el bin de pdf
        } 
      piso_escalon = (escalon / altura_escalon) * altura_escalon; //cambio de escalon

      }
    
    muestra++; //sumo una muestra
    if (muestra >= n_muestras) { //si llega al número de muestras, muestra la PDF y vuelve a cero 
      mostrar_pdf = true;
      muestrear = false;
      muestra = 0;
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

void serialEvent() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      interpretar_comando(comando);
      comando = "";
    } else {
      comando += c;
    }
  }
}

void loop() {
  if (mostrar_pdf) {
    mostrar_pdf = false;
    // Muestra la PDF
    for (int j = 0; j < n_valores; j++) {
      
      escribirDACx(j);
      delayMicroseconds(1);
      
      escribirDACy(pdf[j]);
      delayMicroseconds(1);
    }
    escribirDACx(0);
    // Reset para la próxima medición
    memset(pdf, 0, sizeof(pdf));
    muestrear = true;
  }
}
