int PIN_RED = 5; // GIOP23
int PIN_GREEN = 18; // GIOP22
int PIN_BLUE = 19; // GIOP21
#include "RTClib.h"

RTC_DS3231 rtc;
// Potenciometro conectado a GPIO 35
const int portPin = 13;

// Almacenamiento del valor de puerto (Rango de 0-4095)
int potValor = 0;

const int freq = 5000;
const int redChannel = 0;
const int greenChannel = 1;
const int blueChannel = 2;
const int resolution = 8;
#define clk 35
#define dt 33
int contador = 0; 
int aEstado;
int aUltEstado;  

void setup() {
  
  Serial.begin(57600);
  
  pinMode (clk,INPUT);
  pinMode (dt,INPUT);
  aUltEstado = digitalRead(clk);

  ledcSetup(redChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(PIN_RED, redChannel);

  ledcSetup(greenChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(PIN_GREEN, greenChannel);

  ledcSetup(blueChannel, freq, resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(PIN_BLUE, blueChannel);


#ifndef ESP8266
  while (!Serial); // wait for serial port to connect. Needed for native USB
#endif

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2023, 4, 10, 0, 0, 0));
  }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}
void loop() {
   aEstado = digitalRead(clk); // Leo el estado actual de clk
   // Si el estado previo y el estado actual de CLK son diferentes significa que ocurrio un pulso
   if (aEstado != aUltEstado){     
     // Si el estado de DT es diferente al de CLK significa que se roto 
     if (digitalRead(dt) != aEstado) {
       if (contador<16){
       contador ++;}
     } else {
       if (contador>1){
       contador --;}
       
     }
     Serial.print("Posicion: ");
     Serial.println(contador);
   } 
   aUltEstado = aEstado; // Actualiza el ultimo estado de CLK con el estado actual
DateTime now = rtc.now();
unsigned long seconds = (now.unixtime() % 86400);

setColor(redFunction(seconds),greenFunction(seconds)/contador,blueFunction(seconds)/contador); 
}

void setColor(int R, int G, int B) {
  ledcWrite(redChannel,   R);
  ledcWrite(greenChannel, G);
  ledcWrite(blueChannel,  B);
}

int redFunction(int x){
  int a = int((-1 / 866843099136000000000) * x^5 + (1 / 4299816960000000) * x^4 - (137 / 8360755200000) * x^3 + (13 / 27648000) * x^2 - (33 / 7168) * x + 255);
  if (a>255){return 255;}
  return a;
}

int greenFunction(int x){
  int a = int((191/156031757844480000000000) * x^5 + (-17/96745881600000000) * x^4 + (2089/300987187200000) * x^3 + (-193/1866240000) * x^2 + (24431/6451200) * x + 138);
  if (a>255){return 255;}
  return a;
}

int blueFunction(int x){
  int a = int((131/78015878922240000000000) * x^5 + (-1/3869835264000000) * x^4 + (4829/752467968000000) * x^3 + (739/1866240000) * x^2 + (-90079/9676800) * x + 18);
  if (a>255){return 255;}
  return a;
}
