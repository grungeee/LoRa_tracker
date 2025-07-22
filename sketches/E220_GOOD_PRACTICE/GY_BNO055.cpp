#include "GY_BNO055.h"

// Default I2C address 0x28 or 0x29 depending on the module
Adafruit_BNO055 bno = Adafruit_BNO055(55);

bool initBNO() {
  if (!bno.begin()) {
    Serial.println("BNO055 initialization failed");
    return false;
  }
  bno.setExtCrystalUse(true);
  Serial.println("BNO055 initialized");
  return true;
}

float readHeading() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float heading = -euler.x();
  if (heading < 0) {
    heading += 360.0f;
  }
  return heading;
}

