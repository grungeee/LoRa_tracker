#ifndef GY_BNO055_H
#define GY_BNO055_H

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "LED_Ring.h"

extern Adafruit_BNO055 bno;

void initBNO();
float readHeading();
int headingToLed(float angle);

#endif
