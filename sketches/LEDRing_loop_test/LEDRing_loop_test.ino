#include <Arduino.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 10
#define LED_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LED_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};

  pixels.clear();
  pixels.show();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();

  delay(delayTime);
  pixels.clear();
  pixels.show();
}
// -----------------------------------------

// |================================================|

// Message counter
//int msgCount = 0;

const int buttonPin= 8; // Button connected to D8

//////////////////////////////////////////////////////////////////////////

void setup() {
// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
pixels.begin(); 
pixels.clear();
pixels.show();
// |=============================================|

// |===================< HMS >===================|
  pinMode(buttonPin, INPUT_PULLUP);
// |=============================================|
}



void loop() {
  float segment = 360/16;
  float gxs = 40;
  for (int i=0;i<LED_NUM;i++){
     if  (i * segment <= gxs && gxs <= (i+1) * segment){
     LEDR_COLOR(i,purple, 500);
    }
}






/*
 if (digitalRead(buttonPin) == LOW) {

// --------------- LED RING ----------------
//---------------| Blue: ON |---------------
//------------------------------------------

  LEDR_COLOR(0,blue, 500);
// -----------------------------------------
} else {
// --------------- LED RING ----------------
//---------------| Blue: OFF |--------------
  pixels.clear();
  pixels.show();
// -----------------------------------------
}
*/
}

