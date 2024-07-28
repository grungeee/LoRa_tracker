#include "LED_Ring.h"

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// Color definitions
int red[3] = {255,0,0};
int green[3] = {0,255,0};
int blue[3] = {0,0,255};
int purple[3] = {255,0,255};
int chill[3] = {40,8,40};
int pink[3] = {125,10,10};
int yellow[3] = {255, 255, 0};
int dim_yellow[3] = {20, 20, 0};
int dim_white[3] = {20, 20, 20};

// Function definitions
void initializeLEDRing() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.clear();
  pixels.show();
}

void LEDR_COLOR(int ledIndex, int color[3], int delayTime) {
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
  pixels.clear();
  pixels.show();
}

void LEDcycle(int color[3]) {
  for (int i = 0; i < LEDS_NUM; i++) {
    LEDR_COLOR(i, color, 100);
  }
}



/*
// |==========================< LED RING INCLUDE >===========================|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21 // NOT GOOD FOR SPI
#define LEDS_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};
  int chill[3] = {40,8,40};
  int pink[3] = {125,10,10};
  int yellow[3] = {255, 255, 0};
  int dim_yellow[3] = {20, 20, 0};
  int dim_white[3] = {20, 20, 20};
void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
  pixels.clear();
  pixels.show();
}
// -----------------------------------------
void LEDcycle (int color[3]){
  for (int i=0; i<LEDS_NUM; i++) {
    LEDR_COLOR(i,color,100);
  }
}
    void loopXTimes (int count){
    int loopIterValue = 0;
        for (int i = 0; i < count; i++) {
            loopIterValue = i;
        }
    reutrn loopIterValue;
    }

// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
pixels.begin(); 
pixels.clear();
pixels.show();
// |=============================================|

*/
