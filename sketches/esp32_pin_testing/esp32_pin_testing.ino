#include <Arduino.h>
#include <LoRa_E220.h>
#include "EByte_LoRa_E220_library.h"
#include <HardwareSerial.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 10
#define LEDS_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){

  pixels.clear();
  pixels.show();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
}
// -----------------------------------------

// ===================================================

#define buttonPin 21

// |========================================================================|
///////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
// |=============< LED RING Init >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
pixels.begin(); 
pixels.clear();
pixels.show();
// |=============================================|

// |===================< HMS >===================|
pinMode(buttonPin, INPUT_PULLUP);
  //pinMode(buttonPin_answer, INPUT_PULLUP);
// |=============================================|

///////////////////////////////////////////////////////////////////////////////////////////////////

}
///////////////////////////////////////////////////////////////////////////////////////////////////

// Correct the loop function to handle message sending and receiving properly
void loop() {
  // Set an LED to blue
  LEDR_COLOR(8, blue, 500);

  // Send a message
  // Receive message
  ResponseContainer rc = e220ttl.receiveMessage();
    for (int i = 0; i < LEDS_NUM; i++) {
      LEDR_COLOR(i, green, 100);
  }
}
/*
void loop() {



//-----------------< E220 >----------------
//------------------|Init |---------------
  Serial.println("E220 module initialized.");

//------------------------------------------

// --------------- LED RING ----------------
//---------------| Blue: ON |---------------
//------------------------------------------

  //LEDindex = msgCount;
  LEDR_COLOR(8,blue,500);
// -----------------------------------------

    Serial.println("Sending...[Greet4]");
    e220ttl.sendMessage("Hello, my name is UNO4! Whats your name?");
    delay(500); // DebounAdafruit_NeoPixel library quick functoins descriptionsce delay

  //LEDindex = msgCount;
// --------------- LED RING ----------------
//---------------| Blue: OFF |--------------
//  pixels.clear();
//  pixels.show();
// -----------------------------------------
// Correct the loop function to handle message sending and receiving properly
void loop() {
  // Set an LED to blue
  LEDR_COLOR(8, blue, 500);

  // Send a message
  Serial.println("Sending...[Greet4]");
  e220ttl.sendMessage("Hello, my name is UNO4! What's your name?");
  delay(500);

  // Receive message
  ResponseContainer rc = e220ttl.receiveMessage();
  if (rc.status.code == 1) {
    Serial.println("Receiving...");
    Serial.println(rc.data);

    Serial.println("Answering...");
    e220ttl.sendMessage("Hi, my name is UNO4");

    if (rc.data == "") {
      Serial.println("Can't hear anything...");
    }

    LEDR_COLOR(8, green, 500);
  } else {
    Serial.println("So lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
      LEDR_COLOR(i, green, 100);
    }
  }
}


// --------- E220 reieved data ---------
  ResponseContainer rc = e220ttl.receiveMessage();
// --------- E220 reieved data ---------
  if (rc.status.code == 1) {
    Serial.println("Receiving...");
    Serial.println(rc.data);

    Serial.println("Answerting...");
    e220ttl.sendMessage("Hi, my name is UNO4");
    if (rc.data == "")
      {
      Serial.println("cant hear anything...");
      }

// --------------- LED RING ----------------
 // LEDR_COLOR(8,green, 500);
//pixels.clear();
//pixels.show();
//pixels.setPixelColor(8, pixels.Color(0, 255, 0));
//pixels.show();

//delay(500);
//pixels.clear();
//pixels.show();
// -----------------------------------------

 }
  else {
    Serial.println("so lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
// --------------- LED RING ----------------
      LEDR_COLOR(i,green, 100);
    }
// -----------------------------------------
  }
}

*/
