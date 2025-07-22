#include "Arduino.h"
#include "LoRa_E220.h"
#include "E220.h"
#include "LED_Ring.h"
#include "LCD_GC9A01A.h"
#include <math.h>

struct GpsData {
  float lat;
  float lon;
  float alt;
  uint8_t sat;
};

const float rxLat = 47.963230;
const float rxLon = 16.196234;
const float rxAlt = 279.0;

double calculateDistance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2) {
  const double R = 6371000.0; // meters
  double dLat = radians(lat2 - lat1);
  double dLon = radians(lon2 - lon1);
  double a = sin(dLat/2) * sin(dLat/2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon/2) * sin(dLon/2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  double horizontal = R * c;
  double dAlt = alt2 - alt1;
  return sqrt(horizontal * horizontal + dAlt * dAlt);
}

double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
  double dLon = radians(lon2 - lon1);
  double y = sin(dLon) * cos(radians(lat2));
  double x = cos(radians(lat1)) * sin(radians(lat2)) - sin(radians(lat1)) * cos(radians(lat2)) * cos(dLon);
  double brng = atan2(y, x);
  brng = degrees(brng);
  if (brng < 0) brng += 360.0;
  return brng;
}
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
    LEDR_COLOR(0, dim_blue, 100);
    ResponseStructContainer rsc = e220ttl.receiveMessage(sizeof(GpsData));
    if (rsc.status.code == 1) {
      GpsData data = *(GpsData*)rsc.data;
      double distance = calculateDistance(rxLat, rxLon, rxAlt, data.lat, data.lon, data.alt);
      double bearing = calculateBearing(rxLat, rxLon, data.lat, data.lon);
      Serial.print(F("Lat:")); Serial.print(data.lat, 6);
      Serial.print(F(" Lon:")); Serial.print(data.lon, 6);
      Serial.print(F(" Alt:")); Serial.print(data.alt);
      Serial.print(F(" Sat:")); Serial.print(data.sat);
      Serial.print(F(" Dist:")); Serial.print(distance, 1);
      Serial.print(F(" Dir:")); Serial.println(bearing, 1);
      displayGPSData(data.lat, data.lon, data.alt, data.sat, distance, bearing);

      LEDR_COLOR(0, dim_green, 100);
    } else {
      Serial.println(rsc.status.getResponseDescription());
      LEDR_COLOR(0, dim_red, 100);
    }
    rsc.close();
  }
}

