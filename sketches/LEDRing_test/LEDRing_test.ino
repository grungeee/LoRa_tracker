#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif



#define LED_RING_PIN 10
#define LEDS_COUNT 16

Adafruit_NeoPixel pixels(LEDS_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();



Serial.begin(9600);
// RANDOM NUMBER GEN FROM ANALOG


}

void loop() {

//float randAnal = randomSeed(analogRead(A0));
  //Serial.print(randAnal);

  int r_frac = 0;
  int g_frac = 0;
  int b_frac = 0;

  pixels.clear();

  for(int i=0; i<LEDS_COUNT; i++) {


//  int range = 255/5;
// int col = range * (i+1);
  
if (i == 0) {r_frac = 255; g_frac = 0; b_frac = 0;}
if (i == 1) {r_frac = 0; g_frac = 255; b_frac = 0;} 
if (i == 2) {r_frac = 0; g_frac = 0; b_frac = 255;} 
if (i == 3) {r_frac = 255; g_frac = 0; b_frac = 0;}
if (i == 4) {r_frac = 0; g_frac = 255; b_frac = 0;} 
if (i == 5) {r_frac = 0; g_frac = 0; b_frac = 255;} 
if (i == 6) {r_frac = 255; g_frac = 0; b_frac = 0;}
if (i == 7) {r_frac = 0; g_frac = 255; b_frac = 0;} 
if (i == 8) {r_frac = 0; g_frac = 0; b_frac = 255;} 
if (i == 9) {r_frac = 255; g_frac = 0; b_frac = 0;}
if (i == 10) {r_frac = 0; g_frac = 255; b_frac = 0;} 
if (i == 11) {r_frac = 0; g_frac = 0; b_frac = 255;} 
if (i == 12) {r_frac = 255; g_frac = 0; b_frac = 0;}
if (i == 13) {r_frac = 0; g_frac = 255; b_frac = 0;} 
if (i == 14) {r_frac = 0; g_frac = 0; b_frac = 255;} 
if (i == 15) {r_frac = 255; g_frac = 0; b_frac = 0;}

//r_frac = (i < 5) ? col : 0;
//g_frac = (i >= 5 && i < 10) ? col : 0;
//b_frac = (i >= 10 && i < 16) ? col : 0;




// green
    //pixels.setPixelColor(i, pixels.Color(0, 150, 0));
    pixels.setPixelColor(i, pixels.Color(r_frac, g_frac, b_frac));
    pixels.show();
    delay(DELAYVAL);



  }
}
