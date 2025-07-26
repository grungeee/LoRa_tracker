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

void displayHeading(float heading) {
  clearLCD();
  int16_t x1, y1;
  uint16_t w, h;

  // Draw compass circle and labels
  int cx = 120;
  int cy = 120;
  int radius = 80;
  tft.drawCircle(cx, cy, radius, GC9A01A_WHITE);
  tft.setCursor(cx-5, cy-radius-20);
  tft.print("N");
  tft.setCursor(cx+radius+5-6, cy-3);
  tft.print("E");
  tft.setCursor(cx-3, cy+radius+5);
  tft.print("S");
  tft.setCursor(cx-radius-20, cy-3);
  tft.print("W");

  // Draw heading arrow
  float angle = heading * 0.01745329251; // DEG_TO_RAD
  int len = radius - 10;
  int x2 = cx + (int)(len * sin(angle));
  int y2 = cy - (int)(len * cos(angle));
  tft.drawLine(cx, cy, x2, y2, GC9A01A_RED);

  String text = String(heading, 1) + " deg";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, cy + radius + 20);
  tft.print(text);
}
