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



void displayCompassArrow(float heading, float startHeading) {
  clearLCD();
  int16_t x1, y1;
  uint16_t w, h;

  int cx = 120;
  int cy = 120;
  int radius = 80;
  // Arrow to the starting heading
  float arrowAngle = (startHeading - heading) * 0.01745329251;
  int len = radius - 10;
  int x2 = cx + (int)(len * sin(arrowAngle));
  int y2 = cy - (int)(len * cos(arrowAngle));
  tft.drawLine(cx, cy, x2, y2, GC9A01A_RED);

  String text = String(heading, 1) + " deg";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, cy + radius + 20);
  tft.print(text);
}
//---------------

void displayCompass(float heading, float startHeading) {
  clearLCD();
  int16_t x1, y1;
  uint16_t w, h;

  int cx = 120;
  int cy = 120;
  int radius = 80;
  tft.drawCircle(cx, cy, radius, GC9A01A_WHITE);

  // rotate the compass labels so north stays at the top
  float hRad = heading * 0.01745329251; // DEG_TO_RAD
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

  // Arrow to the starting heading
  float arrowAngle = (startHeading - heading) * 0.01745329251;
  int len = radius - 10;
  int x2 = cx + (int)(len * sin(arrowAngle));
  int y2 = cy - (int)(len * cos(arrowAngle));
  tft.drawLine(cx, cy, x2, y2, GC9A01A_RED);

  String text = String(heading, 1) + " deg";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, cy + radius + 20);
  tft.print(text);
}
