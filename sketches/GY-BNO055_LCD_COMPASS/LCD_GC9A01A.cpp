#include "LCD_GC9A01A.h"

Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

void initLCD() {
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
}

void clearLCD() {
  tft.fillScreen(GC9A01A_BLACK);
}

void displayCompass(float heading, float startHeading) {
  clearLCD();
  int16_t x1, y1;
  uint16_t w, h;

  int cx = 120;
  int cy = 120;
  int radius = 80;
  tft.drawCircle(cx, cy, radius, GC9A01A_WHITE);

  // rotate the compass labels so north stays at the top
  struct { const char *label; float angle; } labels[4] = {
    {"N", 0}, {"E", 90}, {"S", 180}, {"W", 270}
  };

  for (auto &l : labels) {
    float a = (l.angle - heading) * 0.01745329251;
    int tx = cx + (int)((radius + 12) * sin(a)) - 4;
    int ty = cy - (int)((radius + 12) * cos(a)) + 4;
    tft.setCursor(tx, ty);
    tft.print(l.label);
  }

  // Arrow indicating magnetic north
  int len = radius - 10;
  int nx = cx;
  int ny = cy - len;
  uint16_t northColor = GC9A01A_YELLOW;

  // Arrow that starts at boot and rotates with the device
  float arrowAngle = (startHeading - heading) * 0.01745329251;
  int ax = cx + (int)(len * sin(arrowAngle));
  int ay = cy - (int)(len * cos(arrowAngle));

  float diff = fabs(startHeading - heading);
  if (diff > 180.0f) {
    diff = 360.0f - diff;
  }

  uint16_t needleColor = GC9A01A_RED;
  if (diff < 5.0f) {
    northColor = GC9A01A_GREEN;
    needleColor = GC9A01A_GREEN;
  }

  tft.drawLine(cx, cy, nx, ny, northColor);
  tft.drawLine(cx, cy, ax, ay, needleColor);

  String text = String(heading, 1) + " deg";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, cy + radius + 20);
  tft.print(text);
}
