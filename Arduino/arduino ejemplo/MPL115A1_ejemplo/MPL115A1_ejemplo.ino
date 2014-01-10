/************************************************************************
** Código creado en Electrónica 5Hz                                    **
** www.5hz-electronica.com                                             **
** Por:                                                                **
** Basado en el código de:                                             **
** Descripción del código:                                             **
**                                                                     **
** Ejemplo que muestra el funcionamiento del MPL115A1                  **
** en modo SPI                                                         **
*************************************************************************

Conexiones:
UNO   MPL115A1

3.3V       VDD
GND        GND
SDN        pin 7
CSN        pin 10
SDI/MOSI   pin 11
SDO/MISO   pin 12
SCK        pin 13
*/


//-----------------------------------------------------------definiciones

#include <SPI.h> //libreria SPI incluida en el arduino

#define   PRESH   0x80
#define   PRESL   0x82
#define   TEMPH   0x84
#define   TEMPL   0x86

#define   A0MSB   0x88
#define   A0LSB   0x8A
#define   B1MSB   0x8C
#define   B1LSB   0x8E
#define   B2MSB   0x90
#define   B2LSB   0x92
#define   C12MSB  0x94
#define   C12LSB  0x96

#define   CONVERT   0x24   

#define   chipSelectPin 10
#define   shutDown 7

struct barometer 
{
  float presion;
  float temperatura;
};

//--------------------------------------------------definiciones de pines

float A0_;
float B1_;
float B2_;
float C12_;
int opcion=0;

//----------------------------------------------------------configuracion

void setup() 
{
  Serial.begin(115200);
  SPI.begin();   
  // inicializa los pines que controlan el funcionamiento del sensor
  pinMode(shutDown, OUTPUT);
  digitalWrite(shutDown, HIGH);
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  delay (10);
  //Leer registros que contienen los parámetros de chips para hacer los cálculos
  unsigned int A0H = readRegister(A0MSB);
  unsigned int A0L = readRegister(A0LSB);
  A0_ = (A0H << 5) + (A0L >> 3) + (A0L & 0x07) / 8.0;
  
  unsigned int B1H = readRegister(B1MSB);
  unsigned int B1L = readRegister(B1LSB);
  B1_ = ( ( ( (B1H & 0x1F) * 0x100)+B1L) / 8192.0) - 3 ;
  
  unsigned int B2H = readRegister(B2MSB);
  unsigned int B2L = readRegister(B2LSB);
  B2_ = ( ( ( (B2H - 0x80) << 8) + B2L) / 16384.0 ) - 2 ;
  
  unsigned int C12H = readRegister(C12MSB);
  unsigned int C12L = readRegister(C12LSB);
  C12_ = ( ( ( C12H * 0x100 ) + C12L) / 16777216.0 )  ;
          
   menu();
 }
 
 //#######################################################ciclo principal
 
 void loop() 
 {
   if (opcion == 1){
     struct barometer valores2 = baropPessure();
     Serial.print("presion : ");
     Serial.print(valores2.presion);
     Serial.print("  temperatura : ");
     Serial.println(valores2.temperatura);
   }
   delay(1000);
}  

//leer registros
  unsigned int readRegister(byte thisRegister ) {
  unsigned int result = 0;   // result to return
  digitalWrite(chipSelectPin, LOW);
  delay(10);
  SPI.transfer(thisRegister);
  result = SPI.transfer(0x00);
  digitalWrite(chipSelectPin, HIGH);
  return(result);
}

//leer presión
  struct barometer baropPessure(){
  struct barometer valores;
  digitalWrite(chipSelectPin, LOW);
  delay(3);
  SPI.transfer(0x24);
  SPI.transfer(0x00);
  digitalWrite(chipSelectPin, HIGH);
  delay(3);
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(PRESH);
  unsigned int presH = SPI.transfer(0x00);
  delay(3);
  SPI.transfer(PRESL);
  unsigned int presL = SPI.transfer(0x00);
  delay(3);
  SPI.transfer(TEMPH);
  unsigned int tempH = SPI.transfer(0x00);
  delay(3);
  SPI.transfer(TEMPL);
  unsigned int tempL = SPI.transfer(0x00);
  delay(3);
  SPI.transfer(0x00);
  delay(3);
  digitalWrite(chipSelectPin, HIGH);

  unsigned long press = ((presH *256) + presL)/64;
  unsigned long temp  = ((tempH *256) + tempL)/64;

  float pressure = A0_+(B1_+C12_*temp)*press+B2_*temp;
  //float preskPa = pressure*(65.0/1023.0)+50.0;
  valores.presion = pressure*(65.0/1023.0)+50.0;
  valores.temperatura = temp;
  //return(preskPa);
return valores;
}


void menu(){
  int entradaMenu;
  Serial.println("-------------------5HZ ELECTRONICA---------------------------");
  Serial.println("");
  Serial.println("                     PRESENTA");
  Serial.println("");
  Serial.println("*************************************************************");
  Serial.println("");
  Serial.println("      BIEVENIDO AL PROGRAMA DE PRUEBAS DEL MPL115A1");
  Serial.println("");
  Serial.println("*************************************************************");
  Serial.println("");
  Serial.println("MENU PRINCIPAL");
  Serial.println("1- Ver presion barometrica");
  
  do{
  }while(Serial.available()==0);
      entradaMenu=Serial.read();
      switch(entradaMenu)
      {
       case '1':
       Serial.println("");
       Serial.println("La presion barometrica es: ");
       delay(1000);
       opcion = 1;
       break;
       
       default:
       opcion = 0;
       break;
      }
      if (opcion == 1) 
      delay(1000);
}
  
