/*
 _____ ____  ____   ___    _______  __
| ____|___ \|___ \ / _ \  |_   _\ \/ /
|  _|   __) | __) | | | |   | |  \  / 
| |___ / __/ / __/| |_| |   | |  /  \ 
|_____|_____|_____|\___/    |_| /_/\_\
*/

// <===============================<< INCLUDES >================================>
#include "Arduino.h"
//#include "LoRa_E220.h"
//#include <TinyGPSPlus.h>
//#include <HardwareSerial.h>
#include "E220.h" // (from a custom file)
#include "LED_Ring.h" // (from a custom file)
#include "LCD_GC9A01A.h" // (from a custom file
#include "GPS_Module.h" // (from a custom file)


// ------------------------------------------------------------------------------

// <================================<< SETUP >>=================================>
//
void setup() {
// ==========< pre init  >==========|
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for native USB
// ==========<   INIT    >==========|
  initLCD(); 
  initLED();
  initE220();
  configE220();
  initGPS();

  sendE220Message(comChan, "Initialized! ... Hello, world?");
  delay(500);
  LEDcycle(yellow);
}

// <================================<< LOOP >>=================================>
void loop() {
  // |=============< E220 Communication >==============|
receiveE220Message();
  ResponseContainer rc = e220ttl.receiveMessage();
sendE220Message(comChan, "Can you hear me?");
//sendE220Message(comChan, "Please, send me a message!");
  LEDR_COLOR(1, dim_blue, 100);
  if (rc.status.code != 1)
  {
  LEDR_COLOR(15, dim_red, 100);
  }else{
  Serial.println(rc.data);
  LEDR_COLOR(15, dim_green, 100);
  }

  // |================< GPS >==================|
  // Read data from GPS module
  if (gpsSerial.available() > 0)
  {
  LEDR_COLOR(7, dim_blue, 100);
  LEDR_COLOR(8, dim_blue, 100);
  LEDR_COLOR(9, dim_blue, 100);
    // Feed the data into the TinyGPSPlus object
    gps.encode(gpsSerial.read());

    // Display information from the GPS module
    if (gps.location.isUpdated())
    {
    // ===============< LCD >==============|
      displayGPSData(gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
    // ===============< E220 >==============|
      sendE220Message(comChan, getGPSData());
// =================================================

  LEDR_COLOR(7, dim_green, 100);
  LEDR_COLOR(8, dim_green, 100);
  LEDR_COLOR(9, dim_green, 100);
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
  } else {
  LEDR_COLOR(7, dim_red, 100);
  LEDR_COLOR(8, dim_red, 100);
  LEDR_COLOR(9, dim_red, 100);
    displayNoGPSData();
    }

  // |=============< E220 Communication >==============|
receiveE220Message();

  /// end loop ///
}


