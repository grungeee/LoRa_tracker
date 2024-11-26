#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LED_Ring.h" // (from a custom file)

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
  LEDR_COLOR(0, red, 500);
  LEDR_COLOR(0, blue, 500);
  LEDR_COLOR(0, green, 500);
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

   
  float segment = 360/LEDS_NUM;  
  float gxs = event.orientation.x;  
  // float gxs = event.orientation.y; //tilt left/right (reverse )
  // float gxs = event.orientation.z;  //pitch forward/backward(reverse)


  float GX_loop = gxs;
  float x = GX_loop / segment;
  //Serial.print("------------------> ");
  //Serial.println(x);

    if (x > 0) {
      if ((int(x) - x) * (-1) < int(x) + 1 - x) {
        LEDR_COLOR(int(x),purple, 400);
      } else {
        LEDR_COLOR(int(x + 1),purple, 200);
      }
    } else {
      GX_loop = GX_loop * (-1);
      x = GX_loop / 22.5;
      if ((int(x) - x) * (-1) < int(x) + 1 - x) {
        LEDR_COLOR(int(15 - x),red, 400);
      } else {
        LEDR_COLOR(int(15 - (x + 1)),red, 400);
      }
    }

}
