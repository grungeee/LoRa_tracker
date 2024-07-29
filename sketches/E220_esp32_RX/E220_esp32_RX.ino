// /==========================================/ START /===================================/
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "math.h"
#include "LED_Ring.h" // (from a custom file)
// =======================<< LCD >>==============================

#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

#define TFT_CS    5   // Chip select control pin
#define TFT_RST   19
#define TFT_DC    13 // because it is being used by E220
#define TFT_SDA   23  // SPI Data (MOSI)
#define TFT_SCL   18  // SPI Clock (SCK)
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);


// ------------- Funct ---------------
// Function to update the display after the fourth newline
void updateDisplayOnFourthNewline(const String& data) {
    static int newlineCount = 0;
    for (char c : data) {
        if (c == '\n') {
            newlineCount++;  tft.print(text);
  // Print the third variable
  text = "alt: " + String(gps.altitude.meters());
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
            if (newlineCount == 4) {
                // Clear the display area before drawing new text
                tft.fillRect(0, 0, 240, 240, GC9A01A_BLACK);

                // Center text vert  tft.print(text);
  // Print the third variable
  text = "alt: " + String(gps.altitude.meters());
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);ically and horizontally
                int16_t x1, y1;
                uint16_t w, h;
                tft.setTextSize(1);
                tft.setTextColor(GC9A01A_WHITE);

                // Calculate the bounds for the current part of the text
                String text = data.substring(0, data.indexOf('\n', data.indexOf('\n', data.indexOf('\n', data.indexOf('\n') + 1) + 1) + 1));
                tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
                tft.setCursor((240 - w) / 2, (240 - h) / 2);
                tft.print(text);

                // Reset newline count after updating the display
                newlineCount = 0;
            }
        }
    }
}
// ------- LED Ring init -------
//void initializeLEDRing();
//void LEDR_COLOR(int ledIndex, int color[3], int delayTime);
//void LEDcycle(int color[3]);
// |================================================|


#define ENABLE_RSSI true

// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

// ---------- esp32 pins --------------

LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

// =============================<< SETUP >======================================
void setup() {

// ===========< LCD init >==========|
  tft.begin();
  // Rotate display (optional, adjust as needed)
  tft.setRotation(2); // Flip the display the right way  (90 degrees / point)
  // Fill screen with black color
  tft.fillScreen(GC9A01A_BLACK);
  // Set text size
  tft.setTextSize(2);
  // Set text color
  tft.setTextColor(GC9A01A_WHITE);
// ==========< LED Ring init >==========|
initializeLEDRing();

// ------- LED Ring setup init ------- (from a file)
void initializeLEDRing();
// -----------------------------------

  Serial.begin(115200);
    while (!Serial) {}; // wait until serial port is opened

  
  e220ttl.begin(); // Startup all pins and UART

    
// ======================<< SETUP >=================================
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDcycle(yellow);
    Serial.println("YELLOW: Start receiving!"); //init

}


// =============================<< LOOP >======================================
void loop() {
    LEDR_COLOR(0,dim_white,1000);
    delay(1000);
	// If something available
  if (e220ttl.available()>1) {
	  Serial.println("GREEN: Message received!");
    LEDR_COLOR(0,green,500);
    // =================== TEST ANSWER ===================
    // if recieved -> send 
    //ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,"======== DATA RESCIVED ========");
    // if sending cant print for some reason???
    //LEDR_COLOR(8,dim_blue,100);
// ===================================================

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
// ====================< E220 - Received >======================
            //listening ???
    LEDR_COLOR(3,chill, 1000);
    LEDR_COLOR(3,pink, 1000);
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);

// ====================< LCD - Received >====================
updateDisplayOnFourthNewline(rc.data); // data form rc split in 4 lines
            /*
  tft.fillRect(0, 0, 240, 240, GC9A01A_BLACK);
            // Clear the display area before drawing new text
  // Center text vertically and horizontally
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(1);
  tft.setTextColor(GC9A01A_WHITE);
  // Print the first variable
  // Print the second variable
  //text = "lng: " + String(gps.location.lng(), 6);
  String text = rc.data;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 140 - h / 2);
  tft.print(text);
            */

#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
}
///////////////////////////////
