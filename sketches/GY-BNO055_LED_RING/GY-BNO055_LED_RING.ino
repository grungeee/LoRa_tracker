#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

#include "LED_Ring.h" // custom LED ring helper

// The BNO055 is an IMU that fuses gyro, accelerometer and magnetometer
// data internally. We'll use its fused compass heading to control the
// LED ring so the lit LED always points in the same physical direction.

// Create an instance of the sensor
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29); //



//////////////
const float HEADING_OFFSET = 0.0; // add or subtract degrees so LED 0 points north

int calc_led_index(float angle) {
  int leds_count = LEDS_NUM;               // how many LEDs are on the ring
  float segment = 360.0f / leds_count;     // degrees covered by one LED

  // Shift the angle so LED 0 lines up with north.
  // Adding half a segment lets us use integer division to
  // automatically round to the nearest LED position.
  float shifted = angle + HEADING_OFFSET + segment / 2.0f;
  int index = ((int)(shifted / segment)) % leds_count; // convert to 0..(LEDS_NUM-1)

  // The physical LED wiring starts one position clockwise from
  // the angle 0°/north. Bumping the index by one compensates for
  // that so LED 1 lights when facing north.
  index = (index + 1) % leds_count;
  return index + FIRST_LED;                // some rings start at LED 1
}
//////////////

float heading(){
  // Ask the BNO055 for its fused orientation in Euler angles.
  // The x value is yaw (rotation around the vertical axis) which
  // effectively gives us the compass heading in degrees.
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float h = euler.x();  // 0° = north, 90° = east

  // The sensor can return negative angles, e.g. -45° for 315°.
  if (h < 0) {
    h += 360.0f;  // bring into the 0‑360° range
  }

  Serial.print("Heading: ");
  Serial.println(h);

  return h;
}

/////////////////


void setup() {
  // Start serial so we can see debug output in the monitor
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while (1);
  }
  bno.setExtCrystalUse(true);   // use external crystal for better accuracy
  initLED();                    // ready the LED ring
  LEDcycle(pink);               // spin through LEDs on startup
}

void loop() {
  float angle = heading();            // 1) get current heading
  int index = calc_led_index(angle);  // 2) map it to the LED position
  LEDR_COLOR(index, purple, 300);     // 3) light that LED in purple
  delay(1000);  // update about once a second
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
