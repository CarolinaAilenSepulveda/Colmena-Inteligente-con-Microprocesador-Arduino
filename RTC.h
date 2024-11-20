#pragma once
#include <Wire.h>
class RTC_modulo{
    private:
uint8_t segundos;
uint8_t minutos;
uint8_t hora;
uint8_t dia_n;
uint8_t dia_l;
uint8_t mes;
uint8_t año;
public:
RTC_modulo();
~RTCmodulo();
void SetFechayHora(uint8_t dia_n,uint8_t dia_l,uint8_t mes, uint8_t año,uint8_t seg,uint8_t minutos,uint8_t
hora);
void GetSeg();
void GetMinutes();
void GetHora();
void GetDia_n();
void GetDia_l();
void GetMes();
void GetAño();
}