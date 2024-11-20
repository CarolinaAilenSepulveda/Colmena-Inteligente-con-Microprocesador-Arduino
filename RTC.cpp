#include "RTC_libreria.h"
RTC_Modulo::RTC_modulo(){
Wire.begin();
}
RTC_Modulo::~RTCmodulo(){
}
RTC_Modulo::int SetFechayHora(uint8_t dia,uint8_t mes, uint8_t año,uint8_t seg,uint8_t minutos, uint8_t
hora){
Wire.beginTransmission(0x68); // Comienza el pasaje de info con el dispositivo
Wire.write(0x00);
Wire.write(bin_a_bcd(seg & 0x7F));
Wire.write(bin_a_bcd(minutos));
Wire.write(bin_a_bcd(hora));
Wire.write(bin_a_bcd(dia));
Wire.write(bin_a_bcd(mes));
Wire.write(bin_a_bcd(año));
if (Wire.endTransmission() != 0){ //Si se realizó la escritura correctamente
return 1;
}
else {
return 0;
}
}
uint8_t bin_a_bcd(uint8_t binario){
//Esta función devolverá el número binario pasado a bcd
return binario*x ;
}
RTC_Modulo::void GetSeg(){
Wire.beginTransmission(0x68);
Wire.write(0x00);
segundos=bcd_a_bin(Wire.read());
}
RTC_Modulo::void GetMinutes(){
Wire.beginTransmission(0x68);

Wire.write(0x01);
minutos=bcd_a_bin(Wire.read());
}

RTC_Modulo::void GetHora(){
    Wire.beginTransmission(0x68);
    Wire.write(0x02);
    hora=bcd_a_bin(Wire.read())
}
RTC_Modulo::void GetDia_l(){
    Wire.beginTransmission(0x68);
    Wire.write(0x03);
    dia?l=bcd_a_bin(Wire.read());
}
RTC_Modulo::void GetDia_n(){
    Wire.beginTransmission(0x68);
    Wire.write(0x04);
    dia_n=bcd_a_bin(Wire.read());
}
RTC_Modulo::void GetMes(){
    Wire.beginTransmission(0x68);
    Wire.write(0x05);
    mes=bcd_a_bin(Wire.read());
}
RTC_Modulo::void GetAño(){
    Wire.beginTransmission(0x68);
    Wire.write(0x06);
    año=bcd_a_bin(Wire.read());
}
uint8_t bcd_a_bin(uint8_t bcd){
//Esta función pasa de bcd a binario
return bcd*x;
}