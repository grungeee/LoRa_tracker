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

  ResponseStatus initStatus = sendE220Message(comChan, "Initialized! ... Hello, world?");
  Serial.println(initStatus.getResponseDescription());
  delay(500);
  LEDcycle(yellow);
}

// <================================<< LOOP >>=================================>
void loop() {
  // |=============< E220 Communication >==============|
  ResponseContainer rc = receiveE220Message();
  ResponseStatus txStatus = sendE220Message(comChan, "Can you hear me?");
  Serial.println(txStatus.getResponseDescription());
//sendE220Message(comChan, "Please, send me a message!");
  LEDR_COLOR(1, dim_blue, 100);
  if (rc.status.code != E220_SUCCESS)
  {
  LEDR_COLOR(15, dim_red, 100);
  }else{
  Serial.println(rc.data);
  LEDR_COLOR(15, dim_green, 100);
  }

  // |================< GPS >==================|
  // Read data from GPS module using helper
  if (updateGPS())
  {
  LEDR_COLOR(7, dim_blue, 100);
  LEDR_COLOR(8, dim_blue, 100);
  LEDR_COLOR(9, dim_blue, 100);
    // ===============< LCD >==============|
    displayGPSData(gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
    // ===============< E220 >==============|
    ResponseStatus gpsStatus = sendE220Message(comChan, getGPSData());
    Serial.println(gpsStatus.getResponseDescription());

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
  } else {
  LEDR_COLOR(7, dim_red, 100);
  LEDR_COLOR(8, dim_red, 100);
  LEDR_COLOR(9, dim_red, 100);
    displayNoGPSData();
  }

  // |=============< E220 Communication >==============|
  /// end loop ///
}


