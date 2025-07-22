/*
 * Simple tracker demo with E220 LoRa radio, GPS and BNO055 IMU.
 * Sends GPS coordinates and heading over LoRa and shows received
 * data on the serial monitor and LCD. If the GPS is missing or
 * provides no data a warning is printed.
 */

#include "Arduino.h"
#include "E220.h"
#include "LED_Ring.h"
#include "LCD_GC9A01A.h"
#include "GPS_Module.h"
#include "GY_BNO055.h"

unsigned long gpsInitTime = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Initializing LCD...");
  initLCD();
  Serial.println("Initializing LED ring...");
  initLED();
  Serial.println("Initializing E220...");
  initE220();
  configE220();
  Serial.println("Initializing GPS...");
  initGPS();
  gpsInitTime = millis();
  Serial.println("Initializing BNO055...");
  initBNO();

  sendE220Message(comChan, "Initialized");
  LEDcycle(yellow);
}

void loop() {
  // receive incoming messages
  receiveE220Message();
  gpsDataRX();

  bool gpsUpdated = updateGPS();
  bool gpsOk = isGPSAvailable();

  if (gpsUpdated) {
    displayGPSData(gps.location.lat(), gps.location.lng(),
                   gps.altitude.meters(), gps.satellites.value());
    float heading = readHeading();
    int index = headingToLed(heading);
    LEDR_COLOR(index, dim_purple, 50);
    sendE220Message(comChan, getGPSData() + "\nHeading:" + String(heading, 1));
  } else {
    if (!gpsOk && millis() - gpsInitTime > 5000) {
      Serial.println("GPS MODULE NOT FOUND");
      displayNoGPSData();
      sendE220Message(comChan, "NO GPS MODULE");
    } else {
      Serial.println("NO GPS DATA");
      displayNoGPSData();
      sendE220Message(comChan, "NO GPS DATA");
    }
  }

  delay(1000);
}
