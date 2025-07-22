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
#include "E220.h"               // custom LoRa helper
#include "LED_Ring.h"           // LED ring helper
#include "LCD_GC9A01A.h"        // LCD helper
#include "GPS_Module.h"         // GPS helper
#include "GY_BNO055.h"          // BNO055 helper


// ------------------------------------------------------------------------------

// <================================<< SETUP >>=================================>
//
void setup() {
// ==========< pre init  >==========|
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for native USB
// ==========<   INIT    >==========|
  initLCD();
  Serial.println("LCD initialized");
  initLED();
  Serial.println("LED ring initialized");
  initE220();
  Serial.println("E220 initialized");
  configE220();
  Serial.println("E220 configured");
  initGPS();
  Serial.println("GPS initialized");
  initBNO();

  sendE220Message(comChan, "Initialized! ... Hello, world?");
  delay(500);
  LEDcycle(yellow);
}

// <================================<< LOOP >>=================================>
void loop() {
  receiveE220Message();

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  String msg;
  if (gps.location.isUpdated()) {
    displayGPSData(gps.location.lat(), gps.location.lng(), gps.altitude.meters(), gps.satellites.value());
    msg = getGPSData();
  } else if (!gpsDetected()) {
    Serial.println("No GPS detected or unreadable");
    displayNoGPSData();
    msg = "NO GPS DATA";
  } else {
    displayNoGPSData();
    msg = "GPS NOT READY";
  }

  float heading = readHeading();
  Serial.print("Heading: ");
  Serial.println(heading);
  msg += "\nHeading: " + String(heading, 1);

  sendE220Message(comChan, msg);
  delay(1000);
}


