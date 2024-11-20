#include "ModuloCom.h"
Modulo_Comunic::Modulo_Comunic(){};
Modulo_Comunic::~Modulo_Comunic(){};
void ConfigUSART(uint32_t BAUD){

    //Sin paridad
UCSR0C&=~(1<<UMSEL01)&~(UMSEL00);
//Calculo de ubrr con doble velocidad
uint32_t ubrr=(Fosc/(8*BAUD))-1;
UBRR0H=(unsigned char)(ubrr>>8);
UBRR0L=(unsigned char)ubrr;
//Doble velocidad
UCSR0A |= (1<<U2X0);
//8 bits de transmisión
UCSR0C|=(1<<UCSZ01);
UCSR0C|=(1<<UCSZ00);
UCSR0B&=~(1<<UCSZ02);
//1 bit de STOP
UCSR0C&=~(1<<USBS0);
//Habilito enviar y recibir de datos
UCSR0B|=((1<<RXEN0)|(1<<TXEN0));
}
void Modulo_Comunic::Flush(void){
unsigned char basura;
while ((UCSR0A & (1<<RXC0))) basura = UDR0;
}
void Modulo_Comunic::Iniciar(uint32_t BAUD){
ConfigUSART(BAUD);
}
void Modulo_Comunic::Transmitir(unsigned char data){
while (!(UCSR0A & (1<<UDRE0)));
UDR0 = data; //Cargo transmision, se envia
}

int Modulo_Comunic::Recibir(int timeout,unsigned char &data){
int cont=timeout;
while (!(UCSR0A & (1<<RXC0))){
cont--;
_delay_ms(1);
if (cont==0) break;
}
if (cont==0){
return 1;
}
else{
data = (unsigned char)UDR0;
return 0;
}
}
void Modulo_Comunic::TransmitirStr(unsigned char data[]){
for (int i =0; data[i] != '\n'; i++)
Transmitir(data[i]);
}