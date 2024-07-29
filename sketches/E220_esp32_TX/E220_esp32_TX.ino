/*
 _____ ____  ____   ___    _______  __
| ____|___ \|___ \ / _ \  |_   _\ \/ /
|  _|   __) | __) | | | |   | |  \  / 
| |___ / __/ / __/| |_| |   | |  /  \ 
|_____|_____|_____|\___/    |_| /_/\_\
*/

// <================================<< INCLUDES >>=================================>
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
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


// =========================<<GPS>>======================================
TinyGPSPlus gps; // Create an instance of the TinyGPSPlus object
HardwareSerial mySerial(1); // Create an instance of HardwareSerial object
const int RXPin = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 32; // TX pin of ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate
int chan = 23;
String message; //empty string

// ========================<<ESP32>>=====================================
// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

// <================================<< SETUP >>=================================>
//
void setup() {

// ==========< LED Ring init >==========|
initializeLEDRing();

// ==========< GPS Setup  >==========|
  // Start the serial communication with the GPS module
  mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  // Start the serial communication with the computer
  Serial.begin(115200);

  Serial.println("GPS Module testing");
    /////////////////
  delay(500);

// ==========< E220  >==========|
  // Startup all pins and UART
  e220ttl.begin();
    // ------------------------ E220 config ----------------------
	ResponseStructContainer c;
	c = e220ttl.getConfiguration();
	Configuration configuration = *(Configuration*) c.data;
   // ResponseStatus rs = e220ttl.setConfiguration(configuration);
//	configuration.CHAN = 23; // Communication channel
//	configuration.ADDL = 0x03;  // First part of address
//	configuration.ADDH = 0x00; // Second part
    //configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    // ----------------------------------------------


  Serial.println("Hi, I'm going to send message!");

  // Send message
 //ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
 ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,"Hello, world?");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());

  LEDcycle(yellow);
}

// <================================<< LOOP >>=================================>
void loop() {
  // |================< LED Ring >==================|
    // ????? whyyyy
  //LEDR_COLOR(0, chill, 100);
    


  // |================< GPS >==================|
  // Read data from GPS module
  if (mySerial.available() > 0)
  {
    // Feed the data into the TinyGPSPlus object
    gps.encode(mySerial.read());

    // Display information from the GPS module
    if (gps.location.isUpdated())
    {
    message = "\n" + "Latitude: " + String(gps.location.lat(), 6) + "\n" + "Longitude: " + String(gps.location.lng(), 6) + "\n" "Altitude: " + String(gps.altitude.meters()) + "\n" + "Satellites: " + String(gps.satellites.value()) + "\n" + "HDOP: " + String(gps.hdop.value());
    //message = String(gps.location.lat(), 6);
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(chan, message);
//    LEDR_COLOR(8,blue,100);
            /*
        WiFi:
        Latitude: 48.187563
        Longitude: 15.617960
        Altitude: 304.40            /*

     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lng(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lat(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.altitude.meters()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.satellites.value()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.hdop.value()));
            */
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      Serial.print("HDOP: ");
      Serial.println(gps.hdop.value());
    }
  }
  // |================< E220 Communication >==================|
	// If something available
  if (e220ttl.available()>1) {
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
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, input);
	  // Check If there is some problem of succesfully send
	  Serial.println(rs.getResponseDescription());
  }
}
