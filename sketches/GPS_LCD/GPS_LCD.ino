#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "LCD_GC9A01A.h"

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

const int RXPinGPS = 33; // GPS TX -> ESP32 RX
const int TXPinGPS = 32; // GPS RX -> ESP32 TX
const uint32_t GPSBaud = 9600;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
  initLCD();
  bootAnimation();
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {
    displayGPSData(gps.location.lat(), gps.location.lng(),
                   gps.altitude.meters(), gps.satellites.value());
  } else {
    displayNoGPSData();
  }

  delay(1000);
}
