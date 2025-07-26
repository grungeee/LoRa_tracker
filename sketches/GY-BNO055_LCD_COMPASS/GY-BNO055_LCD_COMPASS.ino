#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LCD_GC9A01A.h"

// The BNO055 fuses gyro, accelerometer and magnetometer data internally.
// We'll display the resulting compass heading on the LCD for debugging.
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

// Angle of the on-screen needle. 0 degrees points up.  You can
// change this to use the initial heading or a user selected value
// if desired.
static float needleAngle = 0.0f;

float readHeading() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float h = -euler.x();
  if (h < 0) {
    h += 360.0f;
  }
  Serial.print("Heading: ");
  Serial.println(h);
  return h;
}

void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.println("Ooops, no BNO055 detected ... Check wiring!");
    while (1);
  }
  bno.setExtCrystalUse(true);
  initLCD();
  delay(100); // allow sensor to stabilize
  // Keep needleAngle at 0 so the fixed needle points straight up
}

void loop() {
  float h = readHeading();
  displayCompass(h, needleAngle);
  delay(1000);
}
