#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)

Adafruit_BNO055 bno = Adafruit_BNO055();

void setup() {
  Serial.begin(115200);
  Serial.println("BNO055 Sensor Data");

  if (!bno.begin()) {
    Serial.println("Could not find a valid BNO055 sensor, check wiring!");
    while (1);
  }

  delay(1000);
  
  // Use external crystal for better accuracy
  bno.setExtCrystalUse(true);
}

void loop() {
  // Get sensor events
  sensors_event_t event;
  
  // Get accelerometer data
  bno.getEvent(&event, Adafruit_BNO055::VECTOR_ACCELEROMETER);
  Serial.print("Accel X: ");
  Serial.print(event.acceleration.x);
  Serial.print(" m/s^2, ");
  Serial.print("Y: ");
  Serial.print(event.acceleration.y);
  Serial.print(" m/s^2, ");
  Serial.print("Z: ");
  Serial.print(event.acceleration.z);
  Serial.println(" m/s^2");

  // Get gyroscope data
  bno.getEvent(&event, Adafruit_BNO055::VECTOR_GYROSCOPE);
  Serial.print("Gyro X: ");
  Serial.print(event.gyro.x);
  Serial.print(" rad/s, ");
  Serial.print("Y: ");
  Serial.print(event.gyro.y);
  Serial.print(" rad/s, ");
  Serial.print("Z: ");
  Serial.print(event.gyro.z);
  Serial.println(" rad/s");

  // Get magnetometer data
  bno.getEvent(&event, Adafruit_BNO055::VECTOR_MAGNETOMETER);
  Serial.print("Mag X: ");
  Serial.print(event.magnetic.x);
  Serial.print(" uT, ");
  Serial.print("Y: ");
  Serial.print(event.magnetic.y);
  Serial.print(" uT, ");
  Serial.print("Z: ");
  Serial.print(event.magnetic.z);
  Serial.println(" uT");

  // Get orientation data (Euler angles)
  bno.getEvent(&event, Adafruit_BNO055::VECTOR_EULER);
  Serial.print("Heading: ");
  Serial.print(event.orientation.x);
  Serial.print(" degrees, ");
  Serial.print("Roll: ");
  Serial.print(event.orientation.y);
  Serial.print(" degrees, ");
  Serial.print("Pitch: ");
  Serial.print(event.orientation.z);
  Serial.println(" degrees");

  Serial.println();
  delay(BNO055_SAMPLERATE_DELAY_MS);
}
