
#ifndef GPS_MODULE_H
#define GPS_MODULE_H

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

extern TinyGPSPlus gps;
extern HardwareSerial gpsSerial;

void initGPS();
bool updateGPS();
String getGPSData();
bool isGPSAvailable();

#endif
