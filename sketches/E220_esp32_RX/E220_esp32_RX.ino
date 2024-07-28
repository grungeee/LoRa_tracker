// /==========================================/ START /===================================/
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "math.h"
#include "LED_Ring.h" // (from a custom file)

// ------- LED Ring init -------
void initializeLEDRing();
void LEDR_COLOR(int ledIndex, int color[3], int delayTime);
void LEDcycle(int color[3]);
// |================================================|


#define ENABLE_RSSI true

// ---------- esp32 pins --------------

#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

// =============================<< SETUP >======================================
void setup() {

// ------- LED Ring setup init ------- (from a file)
void initializeLEDRing();
// -----------------------------------

  Serial.begin(115200);
    while (!Serial) {}; // wait until serial port is opened

  
  e220ttl.begin(); // Startup all pins and UART

    
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDcycle(yellow);
    Serial.println("YELLOW: Start receiving!");

}


// =============================<< LOOP >======================================
void loop() {
    LEDR_COLOR(0,dim_white,1000);
    delay(1000);
	// If something available
  if (e220ttl.available()>1) {
	  Serial.println("GREEN:Message received!");

	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220ttl.receiveMessageRSSI();
#else
	ResponseContainer rc = e220ttl.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
    LEDR_COLOR(8,chill, 1000);
    LEDR_COLOR(8,pink, 1000);
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
}
///////////////////////////////
