
// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 10
#define LEDS_COUNT 16
#define LEDnum 0

Adafruit_NeoPixel pixels(LEDS_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// |================================================|

const int buttonPin = 8; // Pin where the button is connected

void setup() {

// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
pixels.begin();
pixels.clear();
pixels.show();
// |=============================================|

  pinMode(buttonPin, INPUT_PULLUP); // Set the pin as input with internal pull-up resistor
  Serial.begin(9600); // Start serial communication for debugging
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
// --------------- LED RING ----------------
  //LEDnum = msgCount;
  pixels.clear();
pixels.show();
  pixels.setPixelColor(LEDnum, pixels.Color(0, 255, 0));
  pixels.show();
// -----------------------------------------
  } else {
// --------------- LED RING ----------------
pixels.clear();
pixels.show();
// -----------------------------------------
  }

  Serial.println(digitalRead(buttonPin)); // Print the state to the serial monitor
    delay(100); // Debounce delay
}
