#include "MPU9250.h"
#include <Wire.h>

// Create an MPU9250 object with the sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire, 0x68);
int status;

void setup() {
  // Initialize serial communication at 115200 baud rate
  Serial.begin(115200);
  while (!Serial) {}  // Wait for the serial port to connect

  // Initialize the IMU
  status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while (1) {}  // Loop indefinitely if initialization fails
  }
}

void loop() {
  // Read data from the IMU sensor
  IMU.readSensor();
  
  // Print accelerometer, gyroscope, magnetometer, and temperature data
  Serial.print(IMU.getAccelX_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getAccelY_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getAccelZ_mss(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroX_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroY_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getGyroZ_rads(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagX_uT(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagY_uT(), 6);
  Serial.print("\t");
  Serial.print(IMU.getMagZ_uT(), 6);
  Serial.print("\t");
  Serial.println(IMU.getTemperature_C(), 6);
  
  delay(100);  // Delay 100 milliseconds between readings
}
