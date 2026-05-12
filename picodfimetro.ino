/*
picodfimetro.ino
PDFimetro basado en Raspberry Pico

Calcula la pdf de una señal entrante en el ADC
Tiene conectados 2 DAC R-2R de 8 bits
Saca la rampa de barrido por DACx
PDF por DACy

Rampa escalonada
PDF con escalones, altura de escalón variable
Ancho de ventana variable
Interfaz por comandos serie (115200 bps)
*/

// Bibliotecas
#include "pico/stdlib.h"
#include "comandos.h"

// Temporizador para el muestreo
repeating_timer_t timer;

//Selección de canal ADC: ADC0 -> GPIO26 ADC1 -> GPIO27
#define ADC_PIN 26

// Frecuencia de muestreo (maximo 500ksps)
const int fclk = 50000;

// 8 bits, resolución ADC = resolución DAC
const int n_valores = 256;

// Contador de muestras
uint32_t muestra = 0;

// Pines asociados a los DACs
const int dacxPins[8] = {2, 3, 4, 5, 6, 7, 8, 9}; 
const int dacyPins[8] = {10,11,12,13,14,15,16,17}; 

// Definición de buffer en memoria
uint8_t pdf[n_valores];

// Lógica de estados
bool mostrar_pdf = false;
bool muestrear = true;

// Parámetros
volatile uint8_t altura_escalon = 10;
volatile uint8_t ancho_ventana = 10;
volatile uint32_t n_muestras = 1000;

// Cadena de comando
String comando = "";

void setup() {
  delay(1000);  //retraso para que no interfiera el DACy con el USB (pin 15)
  
   for (int i = 0; i < 8; i++) {
    pinMode(dacxPins[i], OUTPUT);
    pinMode(dacyPins[i], OUTPUT);
  }
  
  add_repeating_timer_us(-1000000 / fclk, timer_callback, NULL, &timer); //timer para el muestreo
  
  Serial.begin(115200);
  
  comando.reserve(256); // longitud de memoria reservada para el comando
  
  analogReadResolution(8);
}

bool timer_callback(repeating_timer_t *rt) {
  if (muestrear) {
    
    uint8_t v = analogRead(ADC_PIN); //muestrea

    uint8_t escalon = (v / altura_escalon) * altura_escalon; //redondea el valor muestreado a un escalón

    
    if (pdf[escalon] <  n_valores - 1){ //chequea que el bin no esté saturado
      pdf[escalon]++; //incrementa el bin de pdf
      }

    muestra++; //sumo una muestra
    
    if (muestra >= n_muestras) {
      mostrar_pdf = true; // va a iniciar la muestra de la PDF por los DACs
      muestrear = false; // detiene el muestreo (es necesario?)
      muestra = 0; // vuelve a cero el número de muestras
      }
    }  
    
    return true;
}

void escribirDACx(byte valor) {
  for (int i = 0; i < 8; i++) {
    // Extrae cada bit y lo escribe en el pin correspondiente
    digitalWrite(dacxPins[i], (valor >> i) & 1); //desplaza i bits a la derecha, y hace AND con 1
  }
}
void escribirDACy(byte valor) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(dacyPins[i], (valor >> i) & 1);
  }
}

//Comandos: va leyendo los caracteres en el puerto serie, hasta que se encuentra con un salto de línea
void serialEvent() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      interpretar_comando(comando); //función en comandos.cpp
      comando = "";                 //limpia el comando
    } else {
      comando += c;
    }
  }
}

void loop() {
  if (mostrar_pdf) {
    mostrar_pdf = false;
    for (int j = 0; j < n_valores; j++) {
      
      escribirDACx((j / altura_escalon) * altura_escalon); // rampa escalonada
      delayMicroseconds(1);
      
      escribirDACy(pdf[j]); //PDF
      delayMicroseconds(1);
    }
    escribirDACx(0);
    // Reset para la próxima medición
    memset(pdf, 0, sizeof(pdf));
    muestrear = true; // reactiva el muestreo
  }
}
