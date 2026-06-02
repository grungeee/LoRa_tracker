#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <math.h>

// ---------------- Hardware config ----------------
const int LED_RING_PIN = 13;   // WS2812 data pin
const int LED_COUNT = 16;      // 16-LED ring
const int GPS_RX = 33;         // GPS TX -> ESP32 GPIO33
const int GPS_TX = -1;         // Receive-only; do not drive the GPS RX line
const int GPS_RX_FALLBACK = 32;
const uint32_t GPS_BAUD = 9600;
const uint32_t GPS_RX_PROBE_MS = 5000;
// Match sketches/GY-BNO055_LCD_COMPASS wiring:
// BNO055 on default ESP32 I2C pins (SDA=21, SCL=22), address 0x29.
const uint8_t BNO055_ADDR = 0x29;

// Fixed dial shift: this ring's physical indexing needs -270 deg correction.
const float DIAL_SHIFT_CCW_DEG = 180.0f;

// Ring direction mapping:
// true  -> higher heading lights higher LED index
// false -> invert index direction
const bool INDEX_INCREASES_CLOCKWISE = false;
const uint32_t SERIAL_PRINT_INTERVAL_MS = 250;
const uint32_t LOOP_DELAY_MS = 30;
const uint32_t TARGET_BREATH_PERIOD_MS = 4000;  // 0.25 Hz = one full breath every 4 seconds
const float TARGET_MAX_BRIGHTNESS = 0.10f;

// LED colors (GRB order handled by library)
const uint32_t COLOR_NORTH = 0x1A0000;   // 10% red
const uint32_t COLOR_OFF = 0x000000;

Adafruit_NeoPixel ring(LED_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_BNO055 bno(55, BNO055_ADDR);
TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

struct TargetDevice {
  const char *name;
  double lat;
  double lng;
  uint8_t red;
  uint8_t green;
  uint8_t blue;
};

const TargetDevice TARGETS[] = {
  {"green", 47.963525, 16.196262, 0, 255, 0},
  {"blue", 47.963028, 16.196205, 0, 0, 255},
  {"purple", 48.058409, 16.075570, 180, 0, 255},
};
const int TARGET_COUNT = sizeof(TARGETS) / sizeof(TARGETS[0]);

uint32_t lastPrintMs = 0;
uint32_t gpsRawBytes = 0;
uint32_t gpsProbeStartedMs = 0;
uint32_t gpsLastPassedChecksum = 0;
int activeGpsRx = GPS_RX;

void bootLedTest() {
  // One full sweep to verify wiring, order and brightness.
  for (int i = 0; i < LED_COUNT; i++) {
    ring.clear();
    ring.setPixelColor(i, ring.Color(0, 0, 30));  // blue
    ring.show();
    delay(80);
  }
  for (int i = 0; i < LED_COUNT; i++) {
    ring.clear();
    ring.setPixelColor(i, ring.Color(30, 0, 0));  // red
    ring.show();
    delay(50);
  }
  ring.clear();
  ring.show();
}

float normalizeDeg(float value) {
  float wrapped = fmodf(value, 360.0f);
  if (wrapped < 0.0f) {
    wrapped += 360.0f;
  }
  return wrapped;
}

float readHeadingDeg() {
  // Match the display-compass sketch setup: use fused Euler heading.
  // 0° = north, 90° = east, values increase clockwise.
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  return normalizeDeg(euler.x());
}

int headingToLed(float headingDeg) {
  const float step = 360.0f / LED_COUNT;
  float corrected = normalizeDeg(headingDeg);

  int idx = (int)((corrected + step * 0.5f) / step) % LED_COUNT;
  if (!INDEX_INCREASES_CLOCKWISE) {
    idx = (LED_COUNT - idx) % LED_COUNT;
  }
  return idx;
}

uint32_t addRgb(uint32_t baseColor, uint8_t red, uint8_t green, uint8_t blue) {
  uint8_t baseRed = (baseColor >> 16) & 0xFF;
  uint8_t baseGreen = (baseColor >> 8) & 0xFF;
  uint8_t baseBlue = baseColor & 0xFF;

  uint8_t mixedRed = min(255, baseRed + red);
  uint8_t mixedGreen = min(255, baseGreen + green);
  uint8_t mixedBlue = min(255, baseBlue + blue);
  return ring.Color(mixedRed, mixedGreen, mixedBlue);
}

void addPixelColor(int index, uint8_t red, uint8_t green, uint8_t blue) {
  uint32_t current = ring.getPixelColor(index);
  ring.setPixelColor(index, addRgb(current, red, green, blue));
}

float targetBreathLevel(int targetIndex) {
  float phase = (millis() % TARGET_BREATH_PERIOD_MS) / (float)TARGET_BREATH_PERIOD_MS;
  phase = fmodf(phase + targetIndex / (float)TARGET_COUNT, 1.0f);
  return (0.5f - 0.5f * cosf(phase * TWO_PI)) * TARGET_MAX_BRIGHTNESS;
}

void drawCompassLeds(float heading) {
  ring.clear();

  float northIndicatorDeg = normalizeDeg(heading + DIAL_SHIFT_CCW_DEG);
  int northIndex = headingToLed(northIndicatorDeg);
  ring.setPixelColor(northIndex, COLOR_NORTH);

  if (gps.location.isValid()) {
    double localLat = gps.location.lat();
    double localLng = gps.location.lng();

    for (int i = 0; i < TARGET_COUNT; i++) {
      float breath = targetBreathLevel(i);
      double bearing = TinyGPSPlus::courseTo(localLat, localLng, TARGETS[i].lat, TARGETS[i].lng);
      float targetIndicatorDeg = normalizeDeg(heading - (float)bearing + DIAL_SHIFT_CCW_DEG);
      int targetIndex = headingToLed(targetIndicatorDeg);
      addPixelColor(
        targetIndex,
        (uint8_t)(TARGETS[i].red * breath),
        (uint8_t)(TARGETS[i].green * breath),
        (uint8_t)(TARGETS[i].blue * breath)
      );
    }
  }

  ring.show();
}

void updateGps() {
  while (gpsSerial.available()) {
    gpsRawBytes++;
    gps.encode(gpsSerial.read());
  }
}

void startGpsSerial(int rxPin) {
  activeGpsRx = rxPin;
  gpsSerial.end();
  delay(20);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, activeGpsRx, GPS_TX);
  gpsProbeStartedMs = millis();
  gpsLastPassedChecksum = gps.passedChecksum();
  Serial.print(F("GPS UART RX="));
  Serial.print(activeGpsRx);
  Serial.print(F(" TX=off baud="));
  Serial.println(GPS_BAUD);
}

void switchGpsRxIfNeeded() {
  if (gps.passedChecksum() > 0 || activeGpsRx == GPS_RX_FALLBACK) {
    return;
  }
  if (millis() - gpsProbeStartedMs < GPS_RX_PROBE_MS) {
    return;
  }
  if (gps.passedChecksum() == gpsLastPassedChecksum) {
    startGpsSerial(GPS_RX_FALLBACK);
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(F("BNO055 16-LED north pointer"));

  Wire.begin();
  startGpsSerial(GPS_RX);

  ring.begin();
  ring.clear();
  ring.show();
  bootLedTest();

  if (!bno.begin()) {
    Serial.println(F("BNO055 not detected. Check wiring and address 0x29/0x28."));
    while (true) {
      delay(1000);
    }
  }

  bno.setExtCrystalUse(true);
  delay(200);
}

void loop() {
  updateGps();
  switchGpsRxIfNeeded();
  float heading = readHeadingDeg();  // 0=north, increases clockwise

  drawCompassLeds(heading);

  uint8_t sysCal = 0, gyroCal = 0, accelCal = 0, magCal = 0;
  bno.getCalibration(&sysCal, &gyroCal, &accelCal, &magCal);

  uint32_t now = millis();
  if (now - lastPrintMs >= SERIAL_PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    Serial.print(F("heading="));
    Serial.print(heading, 1);
    Serial.print(F(" gps="));
    Serial.print(gps.location.isValid() ? F("fix") : F("no_fix"));
    Serial.print(F(" lat="));
    if (gps.location.isValid()) Serial.print(gps.location.lat(), 6); else Serial.print(F("nan"));
    Serial.print(F(" lon="));
    if (gps.location.isValid()) Serial.print(gps.location.lng(), 6); else Serial.print(F("nan"));
    Serial.print(F(" gps_rx="));
    Serial.print(activeGpsRx);
    Serial.print(F(" sats="));
    Serial.print(gps.satellites.isValid() ? gps.satellites.value() : 0);
    Serial.print(F(" chars="));
    Serial.print(gps.charsProcessed());
    Serial.print(F(" raw="));
    Serial.print(gpsRawBytes);
    Serial.print(F(" pass="));
    Serial.print(gps.passedChecksum());
    Serial.print(F(" fail="));
    Serial.print(gps.failedChecksum());
    Serial.print(F(" baud="));
    Serial.print(GPS_BAUD);
    Serial.print(F(" cal S/G/A/M="));
    Serial.print(sysCal);
    Serial.print('/');
    Serial.print(gyroCal);
    Serial.print('/');
    Serial.print(accelCal);
    Serial.print('/');
    Serial.println(magCal);
  }

  delay(LOOP_DELAY_MS);
}
