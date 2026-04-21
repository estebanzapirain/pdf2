#include "comandos.h"

void interpretar_comando(const String& comando) {
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
      Serial.println("m######: cantidad de muestras MAX=2^32-1");
      break;
    case 'e':
      Serial.printf("ALTURA DE ESCALON =  %u, ANCHO DE VENTANA =  %u \n", altura_escalon, ancho_ventana);
      break;
    case 'h':
      altura_escalon = param;
      n_escalones = 256 / altura_escalon;
      Serial.printf("OK! ALTURA DE ESCALON = %u, Nº ESCALONES = %u\n", altura_escalon, n_escalones);
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
