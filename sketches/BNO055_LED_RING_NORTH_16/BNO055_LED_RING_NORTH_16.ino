#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <math.h>

// ---------------- Hardware config ----------------
const int LED_RING_PIN = 13;   // WS2812 data pin
const int LED_COUNT = 16;      // 16-LED ring
const int BRIGHTNESS_BOOST_PIN = 23;  // Momentary LOW switches targets from 10% to 100%
const int GPS_RX = 33;         // GPS TX -> ESP32 GPIO33
const int GPS_TX = -1;         // Receive-only; do not drive the GPS RX line
const int GPS_RX_FALLBACK = 32;
const uint32_t GPS_BAUD = 9600;
const uint32_t GPS_RX_PROBE_MS = 5000;
// Match sketches/GY-BNO055_LCD_COMPASS wiring:
// BNO055 on default ESP32 I2C pins (SDA=21, SCL=22), address 0x29.
const uint8_t BNO055_ADDR = 0x29;

// Fixed dial shift: this ring's physical indexing needs this correction.
const float DIAL_SHIFT_CCW_DEG = 180.0f;

// Ring direction mapping:
// true  -> higher heading lights higher LED index
// false -> invert index direction
const bool INDEX_INCREASES_CLOCKWISE = false;
const uint32_t SERIAL_PRINT_INTERVAL_MS = 1000;
const uint32_t LOOP_DELAY_MS = 30;
const uint32_t TARGET_BREATH_PERIOD_MS = 4000;  // 0.25 Hz = one full breath every 4 seconds
const uint32_t GREEN_RIPPLE_PERIOD_MS = 900;
const uint32_t GREEN_NORTH_HANDOFF_MS = 10000;
const float TARGET_NORMAL_BRIGHTNESS = 0.10f;
const float TARGET_BOOST_BRIGHTNESS = 1.00f;
const uint32_t GREEN_RX_FRESH_MS = 5000;
const uint8_t ESPNOW_CHANNEL = 1;
const uint8_t DEVICE_NAME_LEN = 12;

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

TargetDevice TARGETS[] = {
  {"green", NAN, NAN, 0, 255, 0},
  {"ADV", NAN, NAN, 255, 90, 0},
  {"blue", 48.00641247087993, 16.2191682545457, 0, 0, 255},
  {"purple", 47.963246, 16.196463, 180, 0, 255},
};
const int TARGET_COUNT = sizeof(TARGETS) / sizeof(TARGETS[0]);

#pragma pack(push, 1)
struct GpsNowPacket {
  char magic[4];
  uint8_t version;
  uint8_t valid;
  uint16_t seq;
  double lat;
  double lon;
  float alt;
  float hdop;
  uint8_t sats;
  uint32_t gpsChars;
  uint32_t uptimeMs;
  char name[DEVICE_NAME_LEN];
};

struct LegacyGpsNowPacket {
  char magic[4];
  uint8_t version;
  uint8_t valid;
  uint16_t seq;
  double lat;
  double lon;
  float alt;
  float hdop;
  uint8_t sats;
  uint32_t gpsChars;
  uint32_t uptimeMs;
};
#pragma pack(pop)

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

uint32_t lastPrintMs = 0;
uint32_t gpsRawBytes = 0;
uint32_t gpsProbeStartedMs = 0;
uint32_t gpsLastPassedChecksum = 0;
uint32_t greenLastRxMs = 0;
uint32_t greenFirstShownMs = 0;
uint32_t anyLastRxMs = 0;
uint32_t espNowRxCount = 0;
uint32_t espNowBadLenCount = 0;
uint32_t espNowBadMagicCount = 0;
uint32_t espNowInvalidGpsCount = 0;
uint16_t greenLastSeq = 0;
uint16_t greenLastPrintedSeq = 0;
uint8_t greenLastSats = 0;
uint32_t targetLastRxMs[TARGET_COUNT] = {};
uint16_t targetLastSeq[TARGET_COUNT] = {};
uint8_t targetLastSats[TARGET_COUNT] = {};
int activeGpsRx = GPS_RX;
bool espNowReady = false;
bool brightnessBoosted = false;
bool lastBoostPinState = false;
uint32_t lastBoostToggleMs = 0;
float greenBearingDeg = NAN;
float greenRingDeg = NAN;
double greenDistanceM = NAN;
int greenLedIndex = -1;

bool validPacket(const GpsNowPacket& packet) {
  return packet.magic[0] == 'G' &&
         packet.magic[1] == 'P' &&
         packet.magic[2] == 'N' &&
         packet.magic[3] == '1' &&
         packet.version == 1;
}

bool plausibleCoordinate(double lat, double lon) {
  return lat >= -90.0 && lat <= 90.0 &&
         lon >= -180.0 && lon <= 180.0 &&
         !(lat == 0.0 && lon == 0.0);
}

int targetIndexForName(const char* name) {
  if (strncmp(name, "ADV", DEVICE_NAME_LEN) == 0) {
    return 1;
  }
  return 0;
}

GpsNowPacket packetFromLegacy(const LegacyGpsNowPacket& legacy) {
  GpsNowPacket packet = {};
  memcpy(packet.magic, legacy.magic, sizeof(packet.magic));
  packet.version = legacy.version;
  packet.valid = legacy.valid;
  packet.seq = legacy.seq;
  packet.lat = legacy.lat;
  packet.lon = legacy.lon;
  packet.alt = legacy.alt;
  packet.hdop = legacy.hdop;
  packet.sats = legacy.sats;
  packet.gpsChars = legacy.gpsChars;
  packet.uptimeMs = legacy.uptimeMs;
  strncpy(packet.name, "green", DEVICE_NAME_LEN);
  return packet;
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onEspNowReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
#else
void onEspNowReceive(const uint8_t* mac, const uint8_t* data, int len) {
#endif
  espNowRxCount++;
  if (len != sizeof(GpsNowPacket) && len != sizeof(LegacyGpsNowPacket)) {
    espNowBadLenCount++;
    return;
  }

  GpsNowPacket packet = {};
  if (len == sizeof(GpsNowPacket)) {
    memcpy(&packet, data, sizeof(packet));
  } else {
    LegacyGpsNowPacket legacy = {};
    memcpy(&legacy, data, sizeof(legacy));
    packet = packetFromLegacy(legacy);
  }

  if (!validPacket(packet)) {
    espNowBadMagicCount++;
    return;
  }

  int targetIndex = targetIndexForName(packet.name);
  uint32_t now = millis();

  anyLastRxMs = now;
  targetLastSeq[targetIndex] = packet.seq;
  targetLastSats[targetIndex] = packet.sats;

  if (!packet.valid || !plausibleCoordinate(packet.lat, packet.lon)) {
    espNowInvalidGpsCount++;
    return;
  }

  TARGETS[targetIndex].lat = packet.lat;
  TARGETS[targetIndex].lng = packet.lon;
  targetLastRxMs[targetIndex] = now;

  if (targetIndex == 0) {
    greenLastRxMs = now;
    greenLastSeq = packet.seq;
    greenLastSats = packet.sats;
  }
}

void setupEspNow() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("LED_RING_ESPNOW", NULL, ESPNOW_CHANNEL, 1);
  WiFi.setSleep(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("BOOT espnow=FAIL"));
    return;
  }
  espNowReady = true;

  esp_now_register_recv_cb(onEspNowReceive);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_AP;

  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus != ESP_OK) {
    Serial.print(F("BOOT espnow_peer=FAIL code="));
    Serial.println(addStatus);
  }

  Serial.print(F("BOOT role=RX espnow=READY ch="));
  Serial.print(ESPNOW_CHANNEL);
  Serial.print(F(" MAC="));
  Serial.println(WiFi.softAPmacAddress());
}

float targetMaxBrightness() {
  return brightnessBoosted ? TARGET_BOOST_BRIGHTNESS : TARGET_NORMAL_BRIGHTNESS;
}

uint32_t northColor() {
  uint8_t red = (uint8_t)(255.0f * targetMaxBrightness());
  return ring.Color(red, 0, 0);
}

void setBootPixel(int index, uint8_t red, uint8_t green, uint8_t blue, float brightness) {
  ring.setPixelColor(
    index,
    ring.Color(
      (uint8_t)(red * brightness),
      (uint8_t)(green * brightness),
      (uint8_t)(blue * brightness)
    )
  );
}

void bootLedTest() {
  for (int frame = 0; frame < 32; frame++) {
    ring.clear();
    int head = frame % LED_COUNT;
    for (int offset = 0; offset < 5; offset++) {
      int idx = (head - offset + LED_COUNT) % LED_COUNT;
      float fade = 0.18f * (5 - offset) / 5.0f;
      setBootPixel(idx, 0, 120, 255, fade);
    }
    ring.show();
    delay(35);
  }

  for (int frame = 0; frame < 24; frame++) {
    float phase = frame / 24.0f;
    for (int i = 0; i < LED_COUNT; i++) {
      float ledPhase = fmodf(phase + i / (float)LED_COUNT, 1.0f);
      float wave = 0.5f - 0.5f * cosf(ledPhase * TWO_PI);
      setBootPixel(i, 180, 0, 255, 0.03f + wave * 0.16f);
    }
    ring.show();
    delay(28);
  }

  for (int step = 0; step < 9; step++) {
    float brightness = (8 - step) / 8.0f * 0.18f;
    for (int i = 0; i < LED_COUNT; i++) {
      setBootPixel(i, 0, 255, 80, brightness);
    }
    ring.show();
    delay(35);
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
  return (0.5f - 0.5f * cosf(phase * TWO_PI)) * targetMaxBrightness();
}

bool blinkOnForDistance(double distanceM) {
  uint32_t periodMs = 0;
  if (distanceM <= 5.0) {
    periodMs = 1000;   // 1 Hz
  } else if (distanceM <= 10.0) {
    periodMs = 2000;   // 1/2 Hz
  } else if (distanceM <= 20.0) {
    periodMs = 3000;   // 1/3 Hz
  } else {
    return true;
  }
  return (millis() % periodMs) < (periodMs / 2);
}

void addScaledTargetColor(int index, const TargetDevice& target, float brightness) {
  addPixelColor(
    index,
    (uint8_t)(target.red * brightness),
    (uint8_t)(target.green * brightness),
    (uint8_t)(target.blue * brightness)
  );
}

float greenRippleSideLevel(int offset, int sideCount) {
  float phase = (millis() % GREEN_RIPPLE_PERIOD_MS) / (float)GREEN_RIPPLE_PERIOD_MS;
  float delayed = phase - offset * 0.16f;
  if (delayed < 0.0f) {
    return 0.0f;
  }

  float wave = 0.0f;
  if (delayed < 0.42f) {
    wave = delayed / 0.42f;
  } else if (delayed < 0.72f) {
    wave = 1.0f - (delayed - 0.42f) / 0.30f;
  }
  wave = constrain(wave, 0.0f, 1.0f);
  wave = wave * wave * (3.0f - 2.0f * wave);

  float falloff = powf(0.52f, offset);
  float rangeScale = sideCount >= 3 ? 1.0f : 0.86f;
  return targetMaxBrightness() * falloff * wave * rangeScale;
}

void drawGreenRipple(int targetIndex, const TargetDevice& target, int sideCount, float centerBrightness) {
  addScaledTargetColor(targetIndex, target, centerBrightness);
  for (int offset = 1; offset <= sideCount; offset++) {
    float level = greenRippleSideLevel(offset, sideCount);
    addScaledTargetColor((targetIndex + offset) % LED_COUNT, target, level);
    addScaledTargetColor((targetIndex - offset + LED_COUNT) % LED_COUNT, target, level);
  }
}

bool drawGreenProximityTarget(int targetIndex, const TargetDevice& target, double distanceM) {
  if (distanceM <= 4.0) {
    int sideCount = distanceM <= 2.0 ? 3 : 2;
    drawGreenRipple(targetIndex, target, sideCount, targetMaxBrightness());
    return true;
  }

  if (distanceM <= 20.0) {
    if (!blinkOnForDistance(distanceM)) {
      return false;
    }
    drawGreenRipple(targetIndex, target, 1, targetMaxBrightness());
    return true;
  }

  addScaledTargetColor(targetIndex, target, targetBreathLevel(0));
  return true;
}

void drawCompassLeds(float heading) {
  ring.clear();

  if (gps.location.isValid()) {
    double localLat = gps.location.lat();
    double localLng = gps.location.lng();

    for (int i = 0; i < TARGET_COUNT; i++) {
      if (isnan(TARGETS[i].lat) || isnan(TARGETS[i].lng)) {
        continue;
      }
      float breath = targetBreathLevel(i);
      double bearing = TinyGPSPlus::courseTo(localLat, localLng, TARGETS[i].lat, TARGETS[i].lng);
      float targetIndicatorDeg = normalizeDeg(heading - (float)bearing + DIAL_SHIFT_CCW_DEG);
      int targetIndex = headingToLed(targetIndicatorDeg);
      if (i == 0) {
        greenBearingDeg = (float)bearing;
        greenRingDeg = targetIndicatorDeg;
        greenDistanceM = TinyGPSPlus::distanceBetween(localLat, localLng, TARGETS[i].lat, TARGETS[i].lng);
        greenLedIndex = targetIndex;
        if (drawGreenProximityTarget(targetIndex, TARGETS[i], greenDistanceM) && greenFirstShownMs == 0) {
          greenFirstShownMs = millis();
        }
        continue;
      }
      addScaledTargetColor(targetIndex, TARGETS[i], breath);
      if (targetLastRxMs[i] > 0 && greenFirstShownMs == 0) {
        greenFirstShownMs = millis();
      }
    }
  }

  bool showNorth = greenFirstShownMs == 0 || millis() - greenFirstShownMs < GREEN_NORTH_HANDOFF_MS;
  if (showNorth) {
    float northIndicatorDeg = normalizeDeg(heading + DIAL_SHIFT_CCW_DEG);
    int northIndex = headingToLed(northIndicatorDeg);
    ring.setPixelColor(northIndex, northColor());
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

void updateBrightnessBoost() {
  bool boostPinState = digitalRead(BRIGHTNESS_BOOST_PIN) == LOW;
  uint32_t now = millis();
  if (boostPinState && !lastBoostPinState && now - lastBoostToggleMs > 250) {
    brightnessBoosted = !brightnessBoosted;
    lastBoostToggleMs = now;
    Serial.print(F("BRIGHTNESS mode="));
    Serial.println(brightnessBoosted ? F("100%") : F("10%"));
  }
  lastBoostPinState = boostPinState;
}

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println(F("BOOT role=RX app=LED_RING"));
  Serial.print(F("BOOT brightness_pin=GPIO"));
  Serial.print(BRIGHTNESS_BOOST_PIN);
  Serial.println(F(" trigger=LOW toggle=10%/100% mode=10%"));

  pinMode(BRIGHTNESS_BOOST_PIN, INPUT_PULLUP);
  Wire.begin();
  startGpsSerial(GPS_RX);
  setupEspNow();

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
  updateBrightnessBoost();
  updateGps();
  switchGpsRxIfNeeded();
  float heading = readHeadingDeg();  // 0=north, increases clockwise

  if (greenLastRxMs > 0 && greenLastSeq != greenLastPrintedSeq) {
    greenLastPrintedSeq = greenLastSeq;
    Serial.print(F("RX_DATA seq="));
    Serial.print(greenLastSeq);
    Serial.print(F(" sats="));
    Serial.print(greenLastSats);
    Serial.print(F(" lat="));
    Serial.print(TARGETS[0].lat, 6);
    Serial.print(F(" lon="));
    Serial.println(TARGETS[0].lng, 6);
  }

  drawCompassLeds(heading);

  uint8_t sysCal = 0, gyroCal = 0, accelCal = 0, magCal = 0;
  bno.getCalibration(&sysCal, &gyroCal, &accelCal, &magCal);

  uint32_t now = millis();
  if (now - lastPrintMs >= SERIAL_PRINT_INTERVAL_MS) {
    lastPrintMs = now;
    Serial.print(F("STATUS role=RX espnow="));
    Serial.print(espNowReady ? (anyLastRxMs > 0 ? F("OK") : F("WAIT")) : F("FAIL"));
    Serial.print(F(" brightness="));
    Serial.print(brightnessBoosted ? F("100") : F("10"));
    Serial.print(F(" pkts="));
    Serial.print(espNowRxCount);
    Serial.print(F(" bad_len="));
    Serial.print(espNowBadLenCount);
    Serial.print(F(" bad_magic="));
    Serial.print(espNowBadMagicCount);
    Serial.print(F(" no_fix="));
    Serial.print(espNowInvalidGpsCount);
    Serial.print(F(" local_gps="));
    Serial.print(gps.location.isValid() ? F("fix") : F("no_fix"));
    Serial.print(F(" sats="));
    Serial.print(gps.satellites.isValid() ? gps.satellites.value() : 0);
    Serial.print(F(" lat="));
    if (gps.location.isValid()) Serial.print(gps.location.lat(), 6); else Serial.print(F("--"));
    Serial.print(F(" lon="));
    if (gps.location.isValid()) Serial.print(gps.location.lng(), 6); else Serial.print(F("--"));
    Serial.print(F(" heading="));
    Serial.print(heading, 1);
    Serial.print(F(" remote="));
    Serial.print(anyLastRxMs > 0 && millis() - anyLastRxMs < GREEN_RX_FRESH_MS ? F("fresh") : F("none"));
    Serial.print(F(" rlat="));
    if (greenLastRxMs > 0) Serial.print(TARGETS[0].lat, 6); else Serial.print(F("--"));
    Serial.print(F(" rlon="));
    if (greenLastRxMs > 0) Serial.print(TARGETS[0].lng, 6); else Serial.print(F("--"));
    Serial.print(F(" rdist="));
    if (isnan(greenDistanceM)) Serial.print(F("--")); else Serial.print(greenDistanceM, 1);
    Serial.print(F(" rbearing="));
    if (isnan(greenBearingDeg)) Serial.print(F("--")); else Serial.print(greenBearingDeg, 1);
    Serial.print(F(" rring="));
    if (isnan(greenRingDeg)) Serial.print(F("--")); else Serial.print(greenRingDeg, 1);
    Serial.print(F(" rled="));
    Serial.print(greenLedIndex);
    Serial.print(F(" seq="));
    Serial.println(greenLastSeq);
  }

  delay(LOOP_DELAY_MS);
}
