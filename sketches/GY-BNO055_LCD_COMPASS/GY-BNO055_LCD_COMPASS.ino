#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LCD_GC9A01A.h"
#include <math.h>

// The BNO055 fuses gyro, accelerometer and magnetometer data internally.
// We'll display the resulting compass heading on the LCD for debugging.
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

float needleHeading = 0.0f;

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
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'l') {
      needleHeading -= 5.0f;
    } else if (c == 'r') {
      needleHeading += 5.0f;
    }
    if (needleHeading > 180.0f) needleHeading -= 360.0f;
    if (needleHeading < -180.0f) needleHeading += 360.0f;
  }

  float h = readHeading();
  bool aligned = fabs(needleHeading) < 5.0f;
  displayCompass(h, needleHeading, aligned);
  delay(200);
}
