#include <Adafruit_NeoPixel.h>//Incluimos la librería en el código

#include "RTClib.h"

RTC_DS3231 rtc;
// Pin del Arduino al cual conectamos el DIN del primer led en la cadena
#define PIN 4 

// Número de píxeles encadenados
#define NUMPIXELS 16 //Modifica este npumero según el arreglo de LEDs con el que cuentes

// Inicializamos nuestro objeto "pixeles"
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int PinAnalogico=2;
int valorLDR;



#define DELAYVAL 500 //timpo de espera en ms 
#define clk 35
#define dt 33
int contador = 1; 
int aEstado;
int aUltEstado;  

void setup() {
  pixels.begin(); // Inicializamos el objeto "pixeles"
  Serial.begin(9600);

  pinMode (clk,INPUT);
  pinMode (dt,INPUT);
  aUltEstado = digitalRead(clk);
  
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

}

void loop() {
  valorLDR=analogRead(PinAnalogico);
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
  int intensidad = levelFunction(valorLDR);
  // El primer pixel de una cadena es el #0, el segundo es el #1, y así sucesivamente
  // hasta el npumero de pixeles menos uno
  for(int i=0; i<NUMPIXELS; i++) { // Para cada pixel...

    
    // Modificamos el LED #i, encendiendolo con un color verde moderadamente brillante
    pixels.setPixelColor(i, pixels.Color(redFunction(seconds)/intensidad,greenFunction(seconds)/intensidad,blueFunction(seconds)/intensidad));
  
    pixels.show(); // Mandamos todos los colores con la actualización hecha

  }
}
int redFunction(int x){
  int a = int((-1 / 866843099136000000000) * x^5 + (1 / 4299816960000000) * x^4 - (137 / 8360755200000) * x^3 + (13 / 27648000) * x^2 - (33 / 7168) * x + 255);
  if (a>255){return 255;}
  return a;
}

int greenFunction(int x){
  int a = int((191/156031757844480000000000) * x^5 + (-17/96745881600000000) * x^4 + (2089/300987187200000) * x^3 + (-193/1866240000) * x^2 + (24431/6451200) * x + 138);
  if (a>255){return 255;}
  return a/1.2;
}

int blueFunction(int x){
  int a = int((131/78015878922240000000000) * x^5 + (-1/3869835264000000) * x^4 + (4829/752467968000000) * x^3 + (739/1866240000) * x^2 + (-90079/9676800) * x + 18);
  if (a>255){return 255;}
  return a/1.2;
}
int levelFunction(int x){
  if(x<256){return 1;}
  if(x<512){return 2;}
  if(x<768){return 3;}
  if(x<1024){return 4;}
  if(x<1280){return 5;}
  if(x<1536){return 6;}
  if(x<1792){return 7;}
  if(x<2048){return 8;}
  if(x<2304){return 9;}
  if(x<2560){return 10;}
  if(x<2816){return 11;}
  if(x<3072){return 12;}
  if(x<3328){return 13;}
  if(x<3584){return 14;}
  if(x<3840){return 15;}
  if(x<=4095){return 16;}
}

