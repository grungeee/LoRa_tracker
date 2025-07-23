  // ======================< LCD >=====================
#include "LCD_GC9A01A.h"

Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

void initLCD() {
  tft.begin();
  // Rotate display (optional, adjust as needed)
  tft.setRotation(2); // Flip the display the right way  (90 degrees / point)
  // Fill screen with black color
  tft.fillScreen(GC9A01A_BLACK);
  // Set text size
  tft.setTextSize(2);
  // Set text color
  tft.setTextColor(GC9A01A_WHITE);
}

void clearLCD() {
  tft.fillScreen(GC9A01A_BLACK);
}

void displayGPSData(float latitude, float longitude, float altitude, int satellites, float distance, float bearing) {

  clearLCD();

  int16_t x1, y1;
  uint16_t w, h;

  String text = "lat: " + String(latitude, 6);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 40 - h / 2);
  tft.print(text);

  text = "lng: " + String(longitude, 6);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 60 - h / 2);
  tft.print(text);

  text = "alt: " + String(altitude);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((205 - w) / 2, 80 - h / 2);
  tft.print(text);

  text = "sat: " + String(satellites);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((145 - w) / 2, 100 - h / 2);
  tft.print(text);

  text = "dist: " + String(distance, 1) + "m";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((185 - w) / 2, 120);
  tft.print(text);

  text = "dir: " + String(bearing, 1);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((175 - w) / 2, 140);
  tft.print(text);

}

void displayNoGPSData() {
  clearLCD();

  int16_t x1, y1;
  uint16_t w, h;

  String text = "NO GPS DATA";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 120 - h / 2);
  tft.print(text);
}

void displayText() {
  clearLCD();

  int16_t x1, y1;
  uint16_t w, h;

  String text = "NO GPS DATA";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 120 - h / 2);
  tft.print(text);
}


void bootAnimation() {
  clearLCD();

  int16_t x1, y1;
  uint16_t w, h;

  String text = "Booting";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 100);
  tft.print(text);

  int barWidth = 160;
  int barHeight = 10;
  int startX = (240 - barWidth) / 2;
  int startY = 130;

  for (int i = 0; i <= barWidth; i += 8) {
    tft.drawRect(startX, startY, barWidth, barHeight, GC9A01A_WHITE);
    tft.fillRect(startX + 1, startY + 1, i, barHeight - 2, GC9A01A_BLUE);
    delay(40);
  }

  delay(300);
  clearLCD();
}


/*
  // Print the second variable
  tft.fillRect(0, 0, 240, 240, GC9A01A_BLACK);
  // Center text vertically and horizontally
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
  // Print the first variable
  String text = "NO GPS DATA";
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  //tft.setCursor((240 - w) / 2, 90 - h / 2);
  tft.setCursor((240 - w) / 2, 120 - h / 2);
  tft.print(text);
*/
