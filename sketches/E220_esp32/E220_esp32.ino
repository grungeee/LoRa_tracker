#include <Arduino.h>
#include <LoRa_E220.h>
#include "EByte_LoRa_E220_library.h"

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
// |================================================|

// Message counter
//int msgCount = 0;

//const int buttonPin= 8; // Button connected to D8
//const int buttonPin_answer = 9; // Button connected to D8

// |===============================< E220 >=================================|
#define E220_30
#define FREQUENCY_868
// Arduino
//LoRa_E220 e220ttl(2, 3, 5, 6, 7);

// ==================< ESP32 >========================
/* 
LoRa_E220(byte txE220pin, byte rxE220pin, HardwareSerial* serial, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600, uint32_t serialConfig = SERIAL_8N1);

LoRa_E220(byte txE220pin, byte rxE220pin, HardwareSerial* serial, byte auxPin, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600, uint32_t serialConfig = SERIAL_8N1);

LoRa_E220(byte txE220pin, byte rxE220pin, HardwareSerial* serial, byte auxPin, byte m0Pin, byte m1Pin, UART_BPS_RATE bpsRate = UART_BPS_RATE_9600, uint32_t serialConfig = SERIAL_8N1);
*/

// ---------- esp32 pins --------------
// LoRa_E220 e220ttl(&Serial2, 15, 21, 19); //  RX AUX M0 M1
 
//LoRa_E220 e220ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e220 TX, esp32 TX --> e220 RX AUX M0 M1
// -------------------------------------
 
//
//  E220: RX  TX  AUX  M1  M0
// ESP32: TX3 RX2 D18 D19 D21
//                D15 (to wake up)
//           3  1            
LoRa_E220 e220ttl(&Serial2, 3, 1, 18, 21, 19);

// ===================================================

void printParameters(struct Configuration configuration);
void printModuleInformation(struct ModuleInformation moduleInformation);
// |========================================================================|
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
  //pinMode(buttonPin, INPUT_PULLUP);
  //pinMode(buttonPin_answer, INPUT_PULLUP);
// |=============================================|

// |===================< E220 >==================|
  Serial.begin(9600);
  e220ttl.begin();
// |=============================================|
}



void loop() {

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

