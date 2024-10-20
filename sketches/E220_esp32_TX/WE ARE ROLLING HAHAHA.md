
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




// ========================<< e220 >>=====================================
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define FREQUENCY_868
// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------
int comChan = 69;
String message; //empty string


// =========================<<GPS>>======================================
TinyGPSPlus gps; // Create an instance of the TinyGPSPlus object
HardwareSerial gpsSerial(1); // Create an instance of HardwareSerial object
const int RXPinGPS = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPinGPS = 32; // TX pin of ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate

// <================================<< SETUP >>=================================>
//
void setup() {
  Serial.begin(115200);
    while (!Serial);
    Serial.println("Serial port OK");

// ==========< LED Ring init >==========|
initializeLEDRing();

// ==========< GPS Setup  >==========|
  
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS); // Start the serial communication with the GPS module

  Serial.println("GPS Module testing");
    /////////////////

// ===========================< E220  >======================|
  // Startup all pins and UART
  e220ttl.begin();
    // -------------------- E220 config ---------------------
ResponseStructContainer c;
c = e220ttl.getConfiguration();
Configuration configuration = *(Configuration*) c.data;
 
configuration.ADDL = BROADCAST_ADDRESS; // OR: 0x03
configuration.ADDH = BROADCAST_ADDRESS; // OR: 0x00
 
configuration.CHAN = 69;
 
configuration.SPED.uartBaudRate = UART_BPS_9600;
configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
configuration.SPED.uartParity = MODE_00_8N1;
 
configuration.OPTION.subPacketSetting = SPS_200_00;
configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
configuration.OPTION.transmissionPower = POWER_22;
 
configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
//configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // FT_FIXED_TRANSMISSION; //repeater mode off
configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION; //repeater mode off
configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
         
e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
c.close();
    // ----------------------------------------------


  Serial.println("Hi, I'm going to send message!");

  // Send message
 //ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
 ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(69,"Hello, world?");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());

  LEDcycle(yellow);
}

// <================================<< LOOP >>=================================>
void loop() {
   LEDR_COLOR(0, dim_white, 1000);
    delay(1000);
  // |================< LED Ring >==================|
    // ????? whyyyy
  //LEDR_COLOR(0, chill, 100);


  // |================< GPS >==================|
  if (gpsSerial.available() > 0)// Read data from GPS module
  {
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
        
//        LEDR_COLOR(5,red,100);
    // Feed the data into the TinyGPSPlus object
        Serial.println("this message was sponsorted by Serial: ");
   Serial.println(gpsSerial.read());
   Serial.println(gpsSerial.available());

    gps.encode(gpsSerial.read());
    // Display information from the GPS module

        Serial.println("this message was sponsorted by GPS:");
      Serial.println(gps.altitude.meters());

        // ??   String message = '';
        str message = "helloooooooooooooooooooo";
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(comChan,">>>>>>" + message);
    if (gps.location.isUpdated())
    {
 //    LEDR_COLOR(5,blue,100);
            /*
    message = "\nLatitude: " + String(gps.location.lat(), 6) + "\nLongitude: " + String(gps.location.lng(), 6) + "\nAltitude: " + String(gps.altitude.meters()) + "\nSatellites: " + String(gps.satellites.value()) + "\nHDOP: " + String(gps.hdop.value());
    */
   // message = String(gps.location.lat(), 6);
    message = gps.satellites.value();

     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(comChan, ">>>>>>" + message);
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
    } else {
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(comChan,"whats gucci??");
    LEDcycle(dim_white);
    delay(1000);
    }
  }
  // |================< E220 Communication >==================|
  if (e220ttl.available()>1) {// If something available
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
