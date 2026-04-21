#pragma once
#include <Arduino.h>

extern volatile uint8_t altura_escalon;
extern volatile uint8_t ancho_ventana;
extern volatile uint8_t n_escalones;
extern volatile uint32_t n_muestras;

void interpretar_comando(const String& comando);
