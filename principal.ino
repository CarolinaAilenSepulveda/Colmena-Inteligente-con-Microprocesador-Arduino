//Librerías

#include <SD.h>
#include <SPI.h>
#include "RTClib.h"
#include <Wire.h>
#include <EEPROM.h>
#include <HX711.h>
#include <LiquidCrystal.h>
#include "ModuloCom.h"
#define Fosc 16000000
#define ADC_Res 1024.0
#define ADC_VREF 5000.0
#include <util/delay.h>

//Declaración de variables globales
volatile int cont_30seg=0;
volatile int cont_1min=0;
int temperatura_prom_1,temperatura_prom_2,contador;
int timeout=1000;
int valor_leido,recibido;
int auxiliar;
int son;
int temperatura_prom=25;
int accion;


int i,band_30seg=0;
int Anio,Mes,Dia,Fecha,Hora,Min;
int seteofecha=0;

unsigned char palabra;
char arreglo[50]={};
char nombre[4]= {'T','P','S','E'};
char estados[4]={'1','2','3','4'};
char est='I';
char arreglo_estados[4]= {'A','B','C','D'};
char M_Temp[8]={'S','E','T','T','E','M','P','\n'};
char M_Fecha[13]={'S','E','T','F','E','C','H','A','H','O','R','A','\n'};
float tempe=0.0;
float pes;

//Declaración de clases
HX711 balanza1;
RTC_DS1307 RTC;
File Archivo;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 6;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Modulo_Comunic Comunicacion;

//Métodos y funciones
void ConfigSD(){
if(!SD.begin(A6)){

//No se inició
Comunicacion.TransmitirStr("No se inició correctamente el SD. \n");
Comunicacion.Transmitir('\n');
while(1);



}
else{
Comunicacion.TransmitirStr("Se configuró el SD. \n");
Comunicacion.Transmitir('\n');
}
}
void Control(){
if (auxiliar > temperatura_prom)
{
PORTB|=(1<<PB0);
PORTB &=~(1<<PB1);
}
else
{
PORTB|=(1<<PB1);
PORTB &=~(1<<PB0);
}
}
void Obtener_p(){
balanza1.begin(A2,A3);
balanza1.set_scale();
float pes_1 = balanza1.read_average(10);
pes=pes_1/838859;
}
void InitADC(){
ADMUX&=~((1<<ADLAR)|(1<<REFS1));
ADMUX|=(1<<REFS0);
//Dejamos activado el convertidor

7

ADCSRA|=(1<<ADEN);
//Prescaler en 128
ADCSRA|=((1<<ADPS0)|(1<<ADPS1)|(1<<ADPS2));
}
int ConvertirADC(){
ADCSRA|=(1<<ADSC); //Comienza la conversión
while ( (ADCSRA & (1<<ADSC)) ){};//Espera a que termine la conversión
//En ADC se guarda el resultado
return ADC;
}
void Obtener_s(){
ADMUX&=~((1<<MUX3)|(1<<MUX2)|(1<<MUX0)|(1<<MUX1));
int son_1 = ConvertirADC();
son=son_1*(10000/1023);
}
void Obtener_t(){
ADMUX&=~((1<<MUX3)|(1<<MUX2)|(1<<MUX1));
ADMUX|=(1<<MUX0);
float t=ConvertirADC()*(ADC_VREF / ADC_Res);
auxiliar=t/10;
}
void Iniciar_lcd(){
lcd.setCursor(0, 0);
lcd.print("Inicio :)");
}
void Menu(float peso,int sonido,char estado,float temperatura){
lcd.setCursor(0, 0);
if (i<4){
i = i+1
if (nombre[i] == "P"){
    lcd.print("Peso:  ");
    lcd.setCursor(0,1);
    lcd.print(peso);
    lcd.print("");
    lcd.print("kg:  ");
}
if (nombre[i] == "S"){
    lcd.print("Sonido: ");
    lcd.setCursor(0,1);
    lcd.print(sonido);
    lcd.print("");
    lcd.print("Hz ");
}
if (nombre[i]=="E"){
    lcd.print("Estado:  ");
    lcd.setCursor(0,1);
    lcd.print(estado);
    lcd.print("  ");

}
else{
    lcd.print("Temperatura: ");
    lcd.setCursor(0,1);
    lcd.print(temperatura);
    lcd.print("   ");
    lcd.print("C  ");
    i =0;
}
    

}

void ConfigT(){
TCCR1A &=~((1<<WGM10)|(1<<WGM11));
TCCR1B&= ~((1<<WGM12)|(1<<WGM13));
TCCR1B|=(1<<CS12)|(1<<CS10);
TCCR1B &=~(1<<CS11) ;
// Habilito Interrupción de desborde
TIMSK1|=(1<<TOIE1);
// Interrupciones generales
sei();
}
int LeerComando(unsigned char mensaje[]){
int ver=0;
if (strncmp(mensaje,M_Temp,7)==0){
ver=1;
}
if (strncmp(mensaje,M_Fecha,12)==0){
ver=2;
}
return ver;
}
void ActEstado(){
if ((pes<0.50)&&(son<3000)){
est='A';
}
if ((pes<0.50)&&(son>4000)){



est='B';
}
if ((pes>1.00)&&(son<3000)){
est='C';
}
if ((pes>1.00)&&(son>4000)){
est='D';
}
}
void CargarConfig(unsigned char mensaje[]){
Archivo=SD.open("Config.txt",FILE_WRITE);
if(Archivo){
Archivo.println(temperatura_prom);
Archivo.close();
}
}
void GuardarDatosSD(DateTime date){
Archivo=SD.open("Datos.txt",FILE_WRITE);
if (Archivo){
Archivo.print("Fecha: ");
Archivo.print(date.day());
Archivo.print('/');
Archivo.print(date.month());
Archivo.print('/');
Archivo.print(date.year());
Archivo.print(" ");
Archivo.print(date.hour());
Archivo.print(':');


Archivo.print(date.minute());
Archivo.print(':');
Archivo.print(date.second());
Archivo.print(" ");
Archivo.print("Peso: ");
Archivo.print(pes);
Archivo.print(" ");
Archivo.print("Sonido: ");
Archivo.print(son);
Archivo.print(" ");
Archivo.print("Temp: ");
Archivo.println(auxiliar);
Archivo.close();
}
}
void CargarDatosRTC(unsigned char mensaje[]){
Anio=((int)arreglo[16]-48)*1000 + ((int)arreglo[17]-48)*100 + ((int)arreglo[18]-48)*10 +
((int)arreglo[19]-48);
Mes=((int)arreglo[14]-48)*10 + ((int)arreglo[15]-48);
Dia= ((int)arreglo[12]-48)*10 + ((int)arreglo[13]-48);
Hora=((int)arreglo[20]-48)*10 + ((int)arreglo[21]-48);
Min=((int)arreglo[22]-48)*10+((int)arreglo[23]-48);
RTC.adjust(DateTime(Anio,Mes,Dia,Hora,Min,00));
char fecha[50]=__DATE__;
int L=strlen(fecha);
fecha[L]='\n';
Comunicacion.TransmitirStr(fecha);
Comunicacion.Transmitir('\n');


char hora[50]=__TIME__;
int ll=strlen(hora);
hora[ll]='\n';
Comunicacion.TransmitirStr(hora);
Comunicacion.Transmitir('\n');
seteofecha=1;
}
//Rutinas de interrupción
ISR(INT0_vect){ //Interrupción botón flecha para ir recorriendo el menú
while(((PIND & (1<<PD2)) >> PD2)==1){}
Menu(pes,son,est,auxiliar);
}
ISR(TIMER1_OVF_vect){
cont_30seg++;
if (cont_30seg>=8){
band_30seg=1;
cont_1min++;
cont_30seg=0;
}
if (cont_1min==2){
//Guardo en SD
//DateTime now=RTC.now();
//GuardarDatosSD(now);
cont_1min=0;
cont_30seg=0;
}
}
//Programa principal - Main



int main(void){
//Declaración entradas y salidas
DDRD&= ~(1<<DDD2);
DDRD&= ~(1<<DDD7);
DDRB|=(1<<DDB0);
DDRB|=(1<<DDB1);
//Habilito interrupción por flanco ascendente para el botón
EICRA &=~(1<<ISC00);
EICRA|=(1<<ISC01);
EIMSK|=(1<<INT0);
sei();
//Llamada a métodos
Comunicacion.Iniciar(9600);
lcd.begin(16, 2);
Iniciar_lcd();
InitADC();
ConfigT();
RTC.begin();
//RTC y guardado en memoria SD
if (!RTC.isrunning()){
Comunicacion.TransmitirStr("Configuración de Fecha y hora actual \n");
Comunicacion.Transmitir('\n');
Comunicacion.TransmitirStr("Escribir el comando: SETFECHAHORA \n");
Comunicacion.Transmitir('\n');
Comunicacion.Transmitir("Solo puede usarse la primera vez \n");
Comunicacion.Transmitir('\n');
Comunicacion.TransmitirStr("Al lado del comando, escribir de la siguiente manera:
AÑOMESDIAHORAMINUTOS\n");



Comunicacion.Transmitir('\n');
}
else{
seteofecha=1;
}
/*ConfigSD();
if (!SD.exists("Config.txt")){
SD.mkdir("Config.txt");
Comunicacion.TransmitirStr("El archivo Config.txt no se encuentra.Se creó uno de cero \n");
Comunicacion.Transmitir('\n');
}
else{ //Cargo la temp nominal
Archivo=SD.open("Config.txt",FILE_READ);
String Linea;
if (Archivo){
while(Archivo.available()){
Linea=Archivo.read();
}
Archivo.close();
}
temperatura_prom=Linea.toInt();
}
if (!SD.exists("Datos.txt")){
SD.mkdir("Datos.txt");
Comunicacion.TransmitirStr("El archivo Datos.csv no se encuentra.Se creó uno de cero \n");
Comunicacion.Transmitir('\n');
}
*/



while(1){
recibido=Comunicacion.Recibir(timeout,palabra);
if (recibido==0){ //Se envio un comando
if (palabra!='\n'){
arreglo[i]=palabra;
i++;
}
else{
arreglo[i]='\n';
Comunicacion.TransmitirStr("Enviaste: \n");
for (int j=0;arreglo[j]!='\n';j++){
Comunicacion.Transmitir(arreglo[j]);
}
Comunicacion.Transmitir('\n');
accion=LeerComando(arreglo); //Identifico el comando
if (accion==0){ //Comando incorrecto
}
if (accion==1){ //Seteo temperatura nominal
temperatura_prom_1=((int)arreglo[7]-48)*10;
temperatura_prom_2=(int)arreglo[8]-48;
temperatura_prom=temperatura_prom_1 + temperatura_prom_2;
//CargarConfig(arreglo);
}
if (accion==2){//Seteo fecha y hora en RTC
if (seteofecha==0){
CargarDatosRTC(arreglo);
}
else{



Comunicacion.TransmitirStr("El RTC ya está configurado, desconectar la pila para volver a configurar
\n");
}
}
i=0;
}
}
else {
//No se recibio ningún comando
}
if (band_30seg==1){
Comunicacion.TransmitirStr("Ya pasaron 30 segundos. Se realizó la medición de los sensores y se
actualizó el LCD\n");
Comunicacion.Transmitir('\n');
Obtener_s();
Obtener_t();
Obtener_p();
ActEstado();
Control();
band_30seg=0;
}
}
return 0;
}