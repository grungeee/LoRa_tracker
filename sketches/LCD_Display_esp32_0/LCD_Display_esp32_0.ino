#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

// Define the pins used for the display
#define TFT_CS    5   // Chip select control pin
#define TFT_RST   19
//#define TFT_DC    4
#define TFT_DC    13 // because it is being used by E220
#define TFT_SDA   23  // SPI Data (MOSI)
#define TFT_SCL   18  // SPI Clock (SCK)
//#define TFT_RST   22  // Reset pin
//#define TFT_DC    21  // Data Command control pin
// Create display object
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

// Define four variables to display
float var1 = 12.34;
float var2 = 56.78;
float var3 = 90.12;
float var4 = 34.56;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  // Initialize the display
  tft.begin();
  // Rotate display (optional, adjust as needed)
  tft.setRotation(3); // Flip the display the right way  (90 degrees / point)
  // Fill screen with black color
  tft.fillScreen(GC9A01A_BLACK);
  // Set text size
  tft.setTextSize(2);
  // Set text color
  tft.setTextColor(GC9A01A_WHITE);
}

void loop() {
  // Clear the display area before drawing new text
  tft.fillRect(0, 0, 240, 240, GC9A01A_BLACK);

  // Center text vertically and horizontally
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);

  // Print the first variable
  String text = "Var1: " + String(var1);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 40 - h / 2);
  tft.print(text);

  // Print the second variable
  text = "Var2: " + String(var2);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 90 - h / 2);
  tft.print(text);

  // Print the third variable
  text = "Var3: " + String(var3);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 140 - h / 2);
  tft.print(text);

  // Print the fourth variable
  text = "Var4: " + String(var4);
  tft.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, 190 - h / 2);
  tft.print(text);

  // Update variables (for demonstration purposes)
  var1 += 0.01;
  var2 += 0.01;
  var3 += 0.01;
  var4 += 0.01;

  // Delay for a second before updating the display
  delay(1000);
}
