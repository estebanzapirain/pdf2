#include "comandos.h"

void interpretar_comando(const String& comando) {
  // Formato de comandos: c#### (un caracter + un parámetro numérico)
  char cmd = comando[0];
  String params = comando.substring(1);
  params.trim();
  int param = params.toInt();
  
  switch (cmd) {
    case '?':
      Serial.println("Ayuda:");
      Serial.println("Ingresar un comando seguido de sus parámetros");
      Serial.println();
      Serial.println("?: Ver ayuda");
      Serial.println("e: Ver estado de los parámetros");
      Serial.println("h####: altura de escalones (en muestras, 1-256)");
      Serial.println("w####: ancho de ventana W (en muestras, 1-256)");
      Serial.println("m####: cantidad de muestras");
      break;
    case 'e':
      Serial.printf("ALTURA DE ESCALON =  %u, ANCHO DE VENTANA =  %u, CANTIDAD DE MUESTRAS =  %u \n", altura_escalon, ancho_ventana, n_muestras);
      break;
    case 'h':
      altura_escalon = param;
      Serial.printf("OK! ALTURA DE ESCALON = %u \n", altura_escalon);
      break;
    case 'w':
      ancho_ventana = param;
      Serial.printf("OK! W: %u \n", ancho_ventana);
      break;
    case 'm':
      n_muestras = param;
      Serial.printf("OK! #muestras: %u \n", n_muestras);
      break;      
  }
}
