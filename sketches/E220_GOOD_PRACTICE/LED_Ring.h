#ifndef LED_RING_H
#define LED_RING_H

#include <Adafruit_NeoPixel.h>

// Pin and LED configuration
#define LED_RING_PIN 25
#define LEDS_NUM 16

extern Adafruit_NeoPixel pixels;

// Color definitions
extern int red[3];
extern int green[3];
extern int blue[3];
extern int purple[3];
extern int yellow[3];
extern int pink[3];
extern int chill[3];
extern int dim_yellow[3];
extern int dim_white[3];
extern int dim_red[3];
extern int dim_green[3];
extern int dim_blue[3];


// Function prototypes
void initLED();
void LEDR_COLOR(int ledIndex, int color[3], int delayTime);
void LEDcycle(int color[3]);

#endif
