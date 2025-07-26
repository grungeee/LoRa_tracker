#include "LCD_GC9A01A.h"

Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

void initLCD() {
  tft.begin();
  tft.setRotation(2);
  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
}

void clearLCD() {
  tft.fillScreen(GC9A01A_BLACK);
}

void displayCompass(float heading, float needleAngle) {
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

  int len = radius - 10;

  float diff = heading - needleAngle;
  if (diff > 180) diff -= 360;
  if (diff < -180) diff += 360;
  bool aligned = fabs(diff) < 5.0f;
  uint16_t northColor = aligned ? GC9A01A_GREEN : GC9A01A_WHITE;
  uint16_t needleColor = aligned ? GC9A01A_GREEN : GC9A01A_RED;

  // Arrow showing magnetic north
  float northAngle = (-heading) * 0.01745329251;
  int nx = cx + (int)(len * sin(northAngle));
  int ny = cy - (int)(len * cos(northAngle));
  tft.drawLine(cx, cy, nx, ny, northColor);

  // Reference needle from screen center
  float refAngle = needleAngle * 0.01745329251;
  int rx = cx + (int)(len * sin(refAngle));
  int ry = cy - (int)(len * cos(refAngle));
  tft.drawLine(cx, cy, rx, ry, needleColor);

  String text = String(heading, 1) + " deg";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, cy + radius + 20);
  tft.print(text);
}
