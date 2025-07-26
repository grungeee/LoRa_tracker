#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

#define TFT_CS    5
#define TFT_RST   19
#define TFT_DC    13

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

float getHeading() {
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float h = -euler.x();
  if (h < 0) {
    h += 360.0f;
  }
  return h;
}

void drawCompass(float deg) {
  tft.fillScreen(GC9A01A_BLACK);
  const int16_t cx = 120;
  const int16_t cy = 120;
  const int16_t r = 90;

  tft.drawCircle(cx, cy, r, GC9A01A_WHITE);
  tft.drawLine(cx, cy - r, cx, cy + r, GC9A01A_DARKGREY);
  tft.drawLine(cx - r, cy, cx + r, cy, GC9A01A_DARKGREY);

  float rad = deg * DEG_TO_RAD;
  int16_t x = cx + r * sin(rad);
  int16_t y = cy - r * cos(rad);
  tft.drawLine(cx, cy, x, y, GC9A01A_RED);

  int16_t hx1 = cx + (r - 10) * sin(rad + 0.15);
  int16_t hy1 = cy - (r - 10) * cos(rad + 0.15);
  int16_t hx2 = cx + (r - 10) * sin(rad - 0.15);
  int16_t hy2 = cy - (r - 10) * cos(rad - 0.15);
  tft.fillTriangle(x, y, hx1, hy1, hx2, hy2, GC9A01A_RED);

  String text = String(deg, 1) + " deg";
  int16_t x1, y1; uint16_t w, h;
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 210);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  tft.print(text);
}

void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.println("No BNO055 detected!");
    while (1);
  }
  bno.setExtCrystalUse(true);

  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(GC9A01A_BLACK);
}

void loop() {
  float h = getHeading();
  Serial.print("Heading: ");
  Serial.println(h);
  drawCompass(h);
  delay(500);
}
