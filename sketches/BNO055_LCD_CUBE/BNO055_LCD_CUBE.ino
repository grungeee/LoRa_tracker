#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

// BNO055 IMU
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

// GC9A01A display pins

#define TFT_CS    5   // Chip select control pin
#define TFT_RST   19
#define TFT_DC    13 // because it is being used by E220
// 
#define TFT_SDA   23  // SPI Data (MOSI)
#define TFT_SCL   18  // SPI Clock (SCK)


Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// -------------------------
void initLCD() {
  tft.begin();
  tft.setRotation(2); // Flip the display the right way  (90 degrees / point)
  // Fill screen with black color
  tft.fillScreen(GC9A01A_BLACK);
  // Set text size
  tft.setTextSize(2);
  // Set text color
  tft.setTextColor(GC9A01A_WHITE);
}


void displayText(String string) {
  tft.fillScreen(GC9A01A_BLACK); // lets clear the screen before we print the text
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(string, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 120 - h / 2);
  tft.print(string);
  }
// -------------------------

// Cube vertex structure
struct Vec3 {
  float x;
  float y;
  float z;
};

// Cube vertices
Vec3 cube[8] = {
  {-1, -1, -1},
  { 1, -1, -1},
  {-1,  1, -1},
  { 1,  1, -1},
  {-1, -1,  1},
  { 1, -1,  1},
  {-1,  1,  1},
  { 1,  1,  1}
};

// Edges between vertices
uint8_t edges[12][2] = {
  {0,1},{1,3},{3,2},{2,0}, // front
  {4,5},{5,7},{7,6},{6,4}, // back
  {0,4},{1,5},{2,6},{3,7}
};

void setup() {
  Serial.begin(115200);
  if(!bno.begin()) {
    Serial.println("No BNO055 detected");
    while(1);
  }
  bno.setExtCrystalUse(true);
  initLCD();
  displayText("booting...");
  delay(1000);
}


// Apply yaw (h), roll (r) and pitch (p) rotations to vector v
Vec3 rotate(Vec3 v, float h, float r, float p) {
  // yaw (around Z)
  float ch = cos(h); float sh = sin(h);
  float x1 = v.x*ch - v.y*sh;
  float y1 = v.x*sh + v.y*ch;
  float z1 = v.z;

  // pitch (around X)
  float cp = cos(p); float sp = sin(p);
  float y2 = y1*cp - z1*sp;
  float z2 = y1*sp + z1*cp;
  float x2 = x1;

  // roll (around Y)
  float cr = cos(r); float sr = sin(r);
  Vec3 out;
  out.x = x2*cr + z2*sr;
  out.z = -x2*sr + z2*cr;
  out.y = y2;
  return out;
}

// Project 3D point to 2D screen coordinates
void project(Vec3 v, int16_t &x, int16_t &y) {
  float distance = 4.0;
  float scale = 80.0;
  float px = v.x/(v.z + distance);
  float py = v.y/(v.z + distance);
  x = (int16_t)(120 + px*scale);
  y = (int16_t)(120 + py*scale);
}

void drawCube(float h, float r, float p) {
  int16_t x[8];
  int16_t y[8];
  for(int i=0;i<8;i++) {
    Vec3 v = rotate(cube[i], h, r, p);
    project(v, x[i], y[i]);
  }
  for(int e=0;e<12;e++) {
    uint8_t a = edges[e][0];
    uint8_t b = edges[e][1];
    tft.drawLine(x[a], y[a], x[b], y[b], GC9A01A_WHITE);
  }
}

void loop() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float heading = euler.x() * DEG_TO_RAD;
  float roll = euler.y() * DEG_TO_RAD;
  float pitch = euler.z() * DEG_TO_RAD;

  // Output raw Euler angles to the serial monitor
  Serial.print("X: ");
  Serial.print(euler.x());
  Serial.print("\tY: ");
  Serial.print(euler.y());
  Serial.print("\tZ: ");
  Serial.println(euler.z());

  tft.fillScreen(GC9A01A_BLACK);
  drawCube(heading, roll, pitch);
//  delay(40);
}
