#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

// Create an instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  sensors_event_t event;
  bno.getEvent(&event);

  // Display the data
  Serial.print("Orientation: ");
  Serial.print("X: "); Serial.print(event.orientation.x); 
  Serial.print(" Y: "); Serial.print(event.orientation.y); 
  Serial.print(" Z: "); Serial.println(event.orientation.z);

  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  Serial.print("Gyroscope: ");
  Serial.print("X: "); Serial.print(gyro.x()); 
  Serial.print(" Y: "); Serial.print(gyro.y()); 
  Serial.print(" Z: "); Serial.println(gyro.z());

  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
  Serial.print("Accelerometer: ");
  Serial.print("X: "); Serial.print(accel.x()); 
  Serial.print(" Y: "); Serial.print(accel.y()); 
  Serial.print(" Z: "); Serial.println(accel.z());

  imu::Vector<3> mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);
  Serial.print("Magnetometer: ");
  Serial.print("X: "); Serial.print(mag.x()); 
  Serial.print(" Y: "); Serial.print(mag.y()); 
  Serial.print(" Z: "); Serial.println(mag.z());

  imu::Vector<3> linaccel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
  Serial.print("Linear Acceleration: ");
  Serial.print("X: "); Serial.print(linaccel.x()); 
  Serial.print(" Y: "); Serial.print(linaccel.y()); 
  Serial.print(" Z: "); Serial.println(linaccel.z());

  imu::Vector<3> gravity = bno.getVector(Adafruit_BNO055::VECTOR_GRAVITY);
  Serial.print("Gravity: ");
  Serial.print("X: "); Serial.print(gravity.x()); 
  Serial.print(" Y: "); Serial.print(gravity.y()); 
  Serial.print(" Z: "); Serial.println(gravity.z());

  delay(1000);
}
