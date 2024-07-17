#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>

#include <Arduino.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h> #endif
#endif

#define LED_RING_PIN 10
#define LEDS_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};

  pixels.clear();
  pixels.show();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();

  delay(delayTime);
  pixels.clear();
  pixels.show();
}
// -----------------------------------------

// |================================================|

// Message counter
//int msgCount = 0;

const int buttonPin= 8; // Button connected to D8

//////////////////////////////////////////////////////////////////////////

/*
SCL: A5
SDA: A4

LED Ring:
N: 
- X: 0
- Y: 0




E 
S
W

*/

MPU6050 mpu;

void setup() {
// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  pixels.begin(); 
  pixels.clear();
  pixels.show();

  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();
  if (mpu.testConnection()) {
      Serial.println("MPU6050 connection successful");
  } else {
      Serial.println("MPU6050 connection failed");
  }

// |=============================================|

// |===================< HMS >===================|
  pinMode(buttonPin, INPUT_PULLUP);
// |=============================================|
}

/*
Axis Drirection:
x ccw
y ccw
z cw
*/

void loop() {


    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);

    float s = 161.0 * 2; // sensitivity scale factor
    float gxs = gx / s;
    float gys = gy / s;
    float gzs = gz / s;

  // ----------- LED 2 -------------
  float segment = 360.0 / LEDS_NUM;
  float GX_loop = gxs * 1;
  float x = GX_loop / 22.5;

  if (x > 0) {
    if ((int(x) - x) * (-1) < int(x) + 1 - x) {
      LEDR_COLOR(int(x),purple, 500);
    } else {
      LEDR_COLOR(int(x + 1),purple, 500);
    }
  } else {
    GX_loop = GX_loop * (-1);
    x = GX_loop / 22.5;
    if ((int(x) - x) * (-1) < int(x) + 1 - x) {
      LEDR_COLOR(int(15 - x),red, 500);
    } else {
      LEDR_COLOR(int(15 - (x + 1)),red, 500);
    }



    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    Serial.println("-----------------\n");
    Serial.print(gxs); Serial.print("\t");
    Serial.print(gys); Serial.print("\t");
    Serial.println(gzs);
    Serial.println("\n-----------------");

    delay(500);
    }
}
