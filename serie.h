#pragma once
#include <avr/io.h>
#include <avr/interrupt.h>
#include "Arduino.h"
#include <stdlib.h>
#define Fosc 16000000
class Modulo_Comunic {
private:
public:
Modulo_Comunic();
~Modulo_Comunic();
void Iniciar(uint32_t BAUD);
int Recibir(int timeout,unsigned char &data);
void Transmitir(unsigned char data);
void TransmitirStr(unsigned char data[]);
void Flush(void);
};