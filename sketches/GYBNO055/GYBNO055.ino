#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Create the BNO055 instance
Adafruit_BNO055 bno = Adafruit_BNO055(55);

void setup() {
  Serial.begin(115200);
  Serial.println("Orientation Sensor Test"); Serial.println("");

  // Initialize the BNO055
  if (!bno.begin()) {
    Serial.print("No BNO055 detected. Check your wiring or I2C ADDR!");
    while (1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  // Get vector data
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  imu::Vector<3> mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

  // Print gyroscopic data
  Serial.print("Gyroscope X: ");
  Serial.print(gyro.x());
  Serial.print(" Y: ");
  Serial.print(gyro.y());
  Serial.print(" Z: ");
  Serial.println(gyro.z());

  // Print magnetometer data
  Serial.print("Magnetometer X: ");
  Serial.print(mag.x());
  Serial.print(" Y: ");
  Serial.print(mag.y());
  Serial.print(" Z: ");
  Serial.println(mag.z());

  delay(100); // Delay a bit to not overload the serial port
}
