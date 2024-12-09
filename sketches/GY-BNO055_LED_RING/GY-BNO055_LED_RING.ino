#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LED_Ring.h" // (from a custom file)

// Create an instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29); //



//////////////
int calc_led_index(float gyX) {
  //reverse
    //gyX = 360 + gyX * -1;
    int leds_count = 12;
    float segment_size = 360.0 / leds_count;  // 30 degrees per segment
    float segment_shift = segment_size / 2.0;

    float LEDindex = gyX / segment_size;
    Serial.println(LEDindex);
    float mod_estLEDindex = fmod(gyX, segment_size);  // gyX % segment_size
    Serial.print("mod ");
    Serial.println(mod_estLEDindex);

    if (LEDindex > 1) {
        LEDindex = floor(LEDindex);
        if (mod_estLEDindex > segment_shift) {
            LEDindex += 1 + FIRST_LED;
        }
    } else {
        if (mod_estLEDindex > segment_shift) {
            LEDindex = 1;
        } else {
            LEDindex = 0;
        }
    }

    Serial.print("LEDindex: ");
    Serial.println(LEDindex);

    return (int)LEDindex;
}
//////////////

float heading(){
  // Get magnetometer data
  imu::Vector<3> mag = bno.getVector(Adafruit_BNO055::VECTOR_MAGNETOMETER);

  // Calculate the heading
  float heading = atan2(mag.y(), mag.x()) * 180.0 / PI;  // Convert to degrees

  // Normalize to 0 - 360 degrees
  if (heading < 0) {
    heading += 360;
  }

  // Print heading information
  Serial.print("Heading: ");
  Serial.println(heading);

  // Check if pointing north (with a tolerance of ±5 degrees)
  /*
  if (heading >= 355 || heading <= 5) {
    Serial.println("You are pointing NORTH!");
  } else {
    Serial.println("Not pointing north.");
  }
  */
  return heading;
}

/////////////////


void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  bno.setExtCrystalUse(true);
  LEDR_COLOR(0, red, 200);
  LEDR_COLOR(0, blue, 200);
  LEDR_COLOR(0, green, 200);
  LEDcycle(pink);
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

   

  float testGyX = 46.0;  // Example input value
  //int index = calc_led_index(event.orientation.x);
  float headingNORTH = heading();
  int index = calc_led_index(headingNORTH);
  //index = index + 1; //(calibration)
  LEDR_COLOR(1, red, 300); //(calibrated)
  LEDR_COLOR(index, purple, 300);
  Serial.print("Index: ");
  Serial.println(index);
  heading();
  delay(1000);  // Wait 1 second before recalculating
}












  /*
  float segment = 360/LEDS_NUM; // segment size
  float gxs = event.orientation.x;  
  // float gxs = event.orientation.y; //tilt left/right (reverse )
  // float gxs = event.orientation.z;  //pitch forward/backward(reverse)


  float GX_loop = gxs; // 0 - 360 ?
  float x = GX_loop / segment; // LED index
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
      x = GX_loop / segment; // 360/16 = 22.5
      if ((int(x) - x) * (-1) < int(x) + 1 - x) {
        LEDR_COLOR(int(15 - x),red, 400);
      } else {
        LEDR_COLOR(int(15 - (x + 1)),red, 400);
      }
    }
  */
