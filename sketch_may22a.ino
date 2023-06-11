#include <Adafruit_NeoPixel.h> //Incluimos la librería del modulo LED en el código

#include "RTClib.h" //Incluimos la librería del reloj en el código

RTC_DS3231 rtc; //Declaracion de la variable reloj

// Pin del boton que cambia de cambio de intensidad por ldr o potenciometro
int boton = 27;
int boton2 = 26;
const int tiempoAntirrebote = 10; // tiempo en ms para evitar el rebote

// Pin del Arduino al cual conectamos el DIN del primer led en la cadena
#define PIN 4

// Número de píxeles encadenados
#define NUMPIXELS 16 //Se modifica este numero según el arreglo de LEDs con el que cuentes

// Inicializamos nuestro objeto "pixeles"
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

//Conexión del ldr
int PinAnalogico=2;

//Inicialización de variables

int valorLDR;
bool selector;

//Definicion de variables del encoder
#define DELAYVAL 500 //timpo de espera en ms 
#define clk 35
#define dt 33
int contador = 1; 
int aEstado;
int aUltEstado;
int estadoBotonAnterior;

void setup() {
  Serial.begin(9600);
  pixels.begin(); // Inicializamos el objeto "pixeles"
  pinMode(boton,  INPUT_PULLUP); //setup del boton
  //Setup del encoder
  pinMode (clk,INPUT);
  pinMode (dt,INPUT);
  aUltEstado = digitalRead(clk);
  //Condiciones del reloj
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
  
  lecturaldr();
  lecturaboton();
  //lecturaboton2();
  lecturaencoder();
  DateTime now = rtc.now();
  unsigned long seconds = (now.unixtime() % 86400); //Se definen los segundos del dia para actualizar el color s/ hora
  int ldr = levelFunction(valorLDR); //Pasa el valor de ldr a uno de los 16 niveles
  int intensidad = seleccionador(selector, ldr, contador); //Se define s/ el valor del boton si se actualiza segun luz ambiente o encoder

  // El primer pixel de una cadena es el #0, el segundo es el #1, y así sucesivamente
  // hasta el npumero de pixeles menos uno
  for(int i=0; i<NUMPIXELS; i++) { // Para cada pixel...

    
    // Modificamos el LED #i, encendiendolo con un color verde moderadamente brillante
    pixels.setPixelColor(i, pixels.Color(255/intensidad,greenFunction(seconds)/intensidad,blueFunction(seconds)/intensidad));
  
    pixels.show(); // Mandamos todos los colores con la actualización hecha

  }
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

void lecturaldr(){
  valorLDR=analogRead(PinAnalogico); //Se lee la información del pin
}

void lecturaboton(){
    int estado = digitalRead(boton); //Lee la informacion del boton
      if(estado != estadoBotonAnterior) //Si el estado cambia es porque se pulsó y se seleccionó la opción contraria
    {
      if(antirrebote(boton)) //Evita el debouncing
      {
        if(selector == true){selector = false;}
        else{selector = true;}
      }
    }  
    estadoBotonAnterior = estado; //Se actualiza el estado del boton
}



void lecturaencoder(){
  aEstado = digitalRead(clk); // Leo el estado actual de clk
   // Si el estado previo y el estado actual de CLK son diferentes significa que ocurrio un pulso
  if (aEstado != aUltEstado){     
    // Si el estado de DT es diferente al de CLK significa que se roto 
    if (digitalRead(dt) != aEstado) {
      if (contador<16){ //Para limitar 16 niveles
      contador ++;} //modifica el contador que cambia la intensidad del led
   } else {
     if (contador>1){
     contador --;}
       
     }
     Serial.print("Posicion: ");
     Serial.println(contador);
   } 
  aUltEstado = aEstado; // Actualiza el ultimo estado de CLK con el estado actual
}


int seleccionador(int estado, int ldr, int contador){
  Serial.println(estado);
  if(estado == HIGH){Serial.println("entré");Serial.println(ldr); return ldr;}
  else{return contador;}
}
  boolean antirrebote(int pin)
  {
    int contador = 0;
    boolean estado;
    boolean estadoAnterior;
   
    do
    {
      estado = digitalRead(pin);
      if(estado != estadoAnterior)
      {
        contador = 0;
        estadoAnterior = estado;
      }
      else
      {
        contador = contador+ 1;
      }
      delay(1);
    } while(contador < tiempoAntirrebote);
   
    return estado;
  }

  int greenFunction(int x){
    if (x<=18000){return 0;}
    if(x<=21600){return 0.0111*x-200;}
    if(x<=32400){return 0.0037*x-40;}
    if(x<=39600){return 0.00806*x-181;}
    if(x<=46800){return 0.00278*x+28;}
   if (x<= 50400){return 0.0081*x-245;}
   if (x<=54000){return -0.00861*x + 623;}
   if(x<=61200){return -0.00278*x + 308;}
   if(x<= 68400){return -0.00806*x + 631;}
   if(x<=75600){return -0.00556*x + 460;}
   return 0;
  }
  int blueFunction(int x){
   if (x<=32400){return 0;}
   if (x<= 39600){return 0.0025*x-81;}
   if(x<=46800){return 0.01*x-378;}
   if(x<=50400){return 0.0194*x-820;}
   if(x<=54000){return -0.0194*x + 1140;}
   if(x<=61200){return -0.01*x + 630;}
   if (x<=68400){return -0.0025*x + 171;}
   return 0;
  }

