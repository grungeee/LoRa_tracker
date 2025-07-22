#ifndef GY_BNO055_H
#define GY_BNO055_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Expose sensor instance so other modules can use it
extern Adafruit_BNO055 bno;

// Initialize the BNO055 sensor. Returns true if successful.
bool initBNO();

// Read the current heading in degrees. Returns NAN on failure.
float readHeading();

#endif

