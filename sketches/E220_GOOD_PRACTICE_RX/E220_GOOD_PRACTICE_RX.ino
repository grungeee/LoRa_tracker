#include "Arduino.h"
#include "LoRa_E220.h"
#include "E220.h"
#include "LED_Ring.h"
#include "LCD_GC9A01A.h"

struct GpsData {
  float lat;
  float lon;
  float alt;
  uint8_t sat;
};

void setup() {
  Serial.begin(115200);
  initLCD();
  initLED();
  initE220();
  configE220();
  LEDcycle(yellow); // indicate startup
}

void loop() {
  if (e220ttl.available() > 1) {
    ResponseStructContainer rsc = e220ttl.receiveMessage(sizeof(GpsData));
    if (rsc.status.code == 1) {
      GpsData data = *(GpsData*)rsc.data;
      Serial.print(F("Lat:")); Serial.print(data.lat, 6);
      Serial.print(F(" Lon:")); Serial.print(data.lon, 6);
      Serial.print(F(" Alt:")); Serial.print(data.alt);
      Serial.print(F(" Sat:")); Serial.println(data.sat);
      displayGPSData(data.lat, data.lon, data.alt, data.sat);
      LEDR_COLOR(0, dim_green, 100);
    } else {
      Serial.println(rsc.status.getResponseDescription());
      LEDR_COLOR(0, dim_red, 100);
    }
    rsc.close();
  }
}

