#include <Arduino.h>
#include <LoRa_E220.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 10
#define LEDS_COUNT 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};

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
const int buttonPin_answer = 9; // Button connected to D8

// |===================< E220 >==================|
#define FREQUENCY_868
LoRa_E220 e220ttl(2, 3, 5, 6, 7);
// |=============================================|
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
  pinMode(buttonPin_answer, INPUT_PULLUP);
// |=============================================|

// |===================< E220 >==================|
  Serial.begin(9600);
  e220ttl.begin();
// |=============================================|
}



void loop() {

 if (digitalRead(buttonPin) == LOW) {

// --------------- LED RING ----------------
//---------------| Blue: ON |---------------
//------------------------------------------

  //LEDindex = msgCount;
  pixels.clear();
  pixels.show();
  pixels.setPixelColor(LEDindex, pixels.Color(0, 0, 255));
  pixels.show();
// -----------------------------------------

    Serial.println("Sending...[Greet4]");
    e220ttl.sendMessage("Hello, my name is UNO4! Whats your name?");
    delay(500); // DebounAdafruit_NeoPixel library quick functoins descriptionsce delay

  //LEDindex = msgCount;
} else {
// --------------- LED RING ----------------
//---------------| Blue: OFF |--------------
  pixels.clear();
  pixels.show();
// -----------------------------------------
}



// --------- E220 reieved data ---------
  ResponseContainer rc = e220ttl.receiveMessage();
// --------- E220 reieved data ---------
  if (rc.status.code == 1) {
   if (digitalRead(buttonPin_answer) == LOW) {
    Serial.println("Receiving...");
    Serial.println(rc.data);

    Serial.println("Answerting...");
    e220ttl.sendMessage("Hi, my name is UNO4");
    if (rc.data == "")
      {
      Serial.println("cant hear anything...");
      }

// --------------- LED RING ----------------
  LEDR_COLOR(8,green, 500);
//pixels.clear();
//pixels.show();
//pixels.setPixelColor(8, pixels.Color(0, 255, 0));
//pixels.show();

//delay(500);
//pixels.clear();
//pixels.show();
// -----------------------------------------
    }

 }
  else {
      Serial.println("so lonely :(");
}
}

