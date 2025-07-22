#ifndef LCD_GC9A01A_H
#define LCD_GC9A01A_H
// =======================<< LCD >>==============================
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

#define TFT_CS    5   // Chip select control pin
#define TFT_RST   19
#define TFT_DC    13 // because it is being used by E220
#define TFT_SDA   23  // SPI Data (MOSI)
#define TFT_SCL   18  // SPI Clock (SCK)

extern Adafruit_GC9A01A tft;

void initLCD();
void clearLCD();

void displayGPSData(float latitude, float longitude, float altitude, int satellites, float distance, float bearing);

void displayNoGPSData();
void displayText();

#endif 
