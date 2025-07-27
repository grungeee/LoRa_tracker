#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LCD_GC9A01A.h"

// The BNO055 fuses gyro, accelerometer and magnetometer data internally.
// We'll display the resulting compass heading on the LCD for debugging.
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

float startHeading = 0.0f;

float readHeading() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float h = euler.x();
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

  uint8_t system, gyro, accel, mag;
  bno.getcalibration(&system, &gyro, &accel, &mag);
  Serial.print("magnetometer cal: ");
  Serial.println(mag); // 0-3, where 3 = fully calibrated

  initReadHeading();
  startHeading = readHeading();

  initLCD();
  delay(100); // allow sensor to stabilize

}

void loop() {

  float h = readHeading();
  displayCompassArrow(h, startHeading);
  delay(1000);
}
