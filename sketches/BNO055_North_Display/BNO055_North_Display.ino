#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <SPI.h>

const int TFT_CS = 5;
const int TFT_RST = 19;
const int TFT_DC = 13;

const int I2C_SDA = 21;
const int I2C_SCL = 22;

const int GPS_RX = 33;  // GPS TX -> ESP32 GPIO33
const int GPS_TX = -1;  // Receive-only; do not drive the GPS RX line
const int GPS_RX_FALLBACK = 32;
const uint32_t GPS_BAUD = 9600;
const uint32_t GPS_RX_PROBE_MS = 5000;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);
Adafruit_BNO055 bno(55, 0x29);
TinyGPSPlus localGps;
HardwareSerial gpsSerial(1);

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
};
#pragma pack(pop)

struct AdvState {
  bool seen = false;
  GpsNowPacket packet = {};
  uint32_t lastMs = 0;
  int8_t rssi = 0;
};

AdvState adv;
uint32_t lastSerialMs = 0;
uint32_t lastDrawMs = 0;
uint32_t lastTransmitMs = 0;
uint32_t gpsRawBytes = 0;
uint32_t gpsProbeStartedMs = 0;
uint32_t gpsLastPassedChecksum = 0;
int activeGpsRx = GPS_RX;
uint16_t sequenceNumber = 0;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

float normalizeDegrees(float degrees) {
  while (degrees < 0.0f) degrees += 360.0f;
  while (degrees >= 360.0f) degrees -= 360.0f;
  return degrees;
}

bool validPacket(const GpsNowPacket& packet) {
  return packet.magic[0] == 'G' &&
         packet.magic[1] == 'P' &&
         packet.magic[2] == 'N' &&
         packet.magic[3] == '1' &&
         packet.version == 1;
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onEspNowReceive(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
#else
void onEspNowReceive(const uint8_t* mac, const uint8_t* data, int len) {
#endif
  Serial.print(F("ESP-NOW Rx len="));
  Serial.println(len);

  if (len != sizeof(GpsNowPacket)) {
    Serial.print(F("Error: expected size "));
    Serial.print(sizeof(GpsNowPacket));
    Serial.print(F(" but got "));
    Serial.println(len);
    return;
  }

  GpsNowPacket packet;
  memcpy(&packet, data, sizeof(packet));
  if (!validPacket(packet)) {
    Serial.print(F("Error: invalid packet magic: "));
    Serial.print(packet.magic[0]);
    Serial.print(packet.magic[1]);
    Serial.print(packet.magic[2]);
    Serial.println(packet.magic[3]);
    return;
  }

  adv.packet = packet;
  adv.seen = true;
  adv.lastMs = millis();
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  adv.rssi = info && info->rx_ctrl ? info->rx_ctrl->rssi : 0;
#endif
}

void setupEspNow() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("ESP_NOW_AP", NULL, 1, 1); // Channel 1, hidden SSID
  WiFi.setSleep(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("ESP-NOW init failed"));
    return;
  }
  
  esp_now_register_recv_cb(onEspNowReceive);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 1;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_AP;
  
  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus != ESP_OK) {
    Serial.print(F("Failed to add peer: "));
    Serial.println(addStatus);
  }

  Serial.print(F("ESP-NOW compass AP channel: "));
  Serial.print(WiFi.channel());
  Serial.print(F(", MAC: "));
  Serial.println(WiFi.softAPmacAddress());
}

double distanceToAdvMeters() {
  if (!localGps.location.isValid() || !adv.seen || !adv.packet.valid) return -1.0;

  double horizontal = TinyGPSPlus::distanceBetween(
    localGps.location.lat(),
    localGps.location.lng(),
    adv.packet.lat,
    adv.packet.lon
  );
  double localAlt = localGps.altitude.isValid() ? localGps.altitude.meters() : 0.0;
  double dAlt = adv.packet.alt - localAlt;
  return sqrt(horizontal * horizontal + dAlt * dAlt);
}

double bearingToAdvDeg() {
  if (!localGps.location.isValid() || !adv.seen || !adv.packet.valid) return NAN;
  return TinyGPSPlus::courseTo(
    localGps.location.lat(),
    localGps.location.lng(),
    adv.packet.lat,
    adv.packet.lon
  );
}

void drawCenteredText(const String& text, int16_t y, uint16_t color, uint8_t size = 2) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.getTextBounds(text, 0, y, &x1, &y1, &w, &h);
  tft.setCursor((240 - w) / 2, y);
  tft.print(text);
}

void drawArrow(float relativeDeg, int len, uint16_t color, int tailLen = 22, int radius = 5) {
  const int cx = 120;
  const int cy = 120;
  float angle = relativeDeg * DEG_TO_RAD;
  int xTip = cx + (int)(len * sin(angle));
  int yTip = cy - (int)(len * cos(angle));
  int xTail = cx - (int)(tailLen * sin(angle));
  int yTail = cy + (int)(tailLen * cos(angle));
  tft.drawLine(xTail, yTail, xTip, yTip, color);
  tft.drawLine(xTail + 1, yTail, xTip + 1, yTip, color);
  tft.fillCircle(xTip, yTip, radius, color);
}

void drawCompass(float headingDeg, uint8_t sysCal, uint8_t gyroCal, uint8_t accelCal, uint8_t magCal) {
  const int cx = 120;
  const int cy = 120;
  const int radius = 104;

  tft.fillScreen(GC9A01A_BLACK);
  tft.drawCircle(cx, cy, radius, GC9A01A_DARKGREY);
  tft.drawCircle(cx, cy, radius - 1, GC9A01A_DARKGREY);
  tft.drawFastVLine(cx, cy - radius, 10, GC9A01A_DARKGREY);
  tft.drawFastVLine(cx, cy + radius - 10, 10, GC9A01A_DARKGREY);
  tft.drawFastHLine(cx - radius, cy, 10, GC9A01A_DARKGREY);
  tft.drawFastHLine(cx + radius - 10, cy, 10, GC9A01A_DARKGREY);

  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setCursor(112, 12); tft.print("N");
  tft.setCursor(112, 210); tft.print("S");
  tft.setCursor(210, 113); tft.print("E");
  tft.setCursor(18, 113); tft.print("W");

  drawArrow(normalizeDegrees(360.0f - headingDeg), 72, GC9A01A_RED, 24, 6);

  double advBearing = bearingToAdvDeg();
  double advDistance = distanceToAdvMeters();
  bool advFresh = adv.seen && millis() - adv.lastMs < 5000;
  if (!isnan(advBearing) && advFresh) {
    float relativeAdv = normalizeDegrees((float)advBearing - headingDeg);
    drawArrow(relativeAdv, 52, GC9A01A_GREEN, 16, 5);
  }

  tft.fillCircle(cx, cy, 5, GC9A01A_CYAN);
  drawCenteredText(String(headingDeg, 1) + " deg", 66, GC9A01A_YELLOW, 2);

  tft.setTextSize(1);
  tft.setTextColor(GC9A01A_RED);
  tft.setCursor(45, 150);
  tft.print("RED north");
  tft.setTextColor(GC9A01A_GREEN);
  tft.setCursor(45, 162);
  tft.print("GREEN ADV ");
  if (advDistance >= 0.0 && advFresh) {
    if (advDistance >= 1000.0) {
      tft.print(advDistance / 1000.0, 2);
      tft.print("km");
    } else {
      tft.print(advDistance, 0);
      tft.print("m");
    }
  } else {
    tft.print("--");
  }

  tft.setTextColor(GC9A01A_WHITE);
  tft.setCursor(45, 176);
  tft.print("GPS ");
  tft.print(localGps.location.isValid() ? "fix" : "no");
  tft.print(" ADV ");
  tft.print(advFresh ? "rx" : "wait");

  if (localGps.location.isValid()) {
    tft.setCursor(45, 188);
    tft.print("LAT ");
    tft.print(localGps.location.lat(), 6);
    tft.setCursor(45, 200);
    tft.print("LON ");
    tft.print(localGps.location.lng(), 6);
  }

  tft.setTextColor(GC9A01A_GREEN);
  tft.setCursor(48, 212);
  tft.print("cal S");
  tft.print(sysCal);
  tft.print(" G");
  tft.print(gyroCal);
  tft.print(" A");
  tft.print(accelCal);
  tft.print(" M");
  tft.print(magCal);
}

void printStatus(float headingDeg, uint8_t sysCal, uint8_t gyroCal, uint8_t accelCal, uint8_t magCal) {
  imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
  double advBearing = bearingToAdvDeg();
  double advDistance = distanceToAdvMeters();

  Serial.print(F("heading_deg="));
  Serial.print(headingDeg, 2);
  Serial.print(F(" north_arrow_deg="));
  Serial.print(normalizeDegrees(360.0f - headingDeg), 2);
  Serial.print(F(" adv_bearing_deg="));
  if (isnan(advBearing)) Serial.print(F("nan")); else Serial.print(advBearing, 2);
  Serial.print(F(" adv_distance_m="));
  if (advDistance < 0.0) Serial.print(F("nan")); else Serial.print(advDistance, 1);
  Serial.print(F(" local_gps="));
  Serial.print(localGps.location.isValid());
  Serial.print(F(" lat="));
  if (localGps.location.isValid()) Serial.print(localGps.location.lat(), 6); else Serial.print(F("nan"));
  Serial.print(F(" lon="));
  if (localGps.location.isValid()) Serial.print(localGps.location.lng(), 6); else Serial.print(F("nan"));
  Serial.print(F(" gps_rx="));
  Serial.print(activeGpsRx);
  Serial.print(F(" gps_raw="));
  Serial.print(gpsRawBytes);
  Serial.print(F(" gps_chars="));
  Serial.print(localGps.charsProcessed());
  Serial.print(F(" gps_pass="));
  Serial.print(localGps.passedChecksum());
  Serial.print(F(" gps_fail="));
  Serial.print(localGps.failedChecksum());
  Serial.print(F(" gps_sats="));
  Serial.print(localGps.satellites.isValid() ? localGps.satellites.value() : 0);
  Serial.print(F(" adv_seen="));
  Serial.print(adv.seen);
  Serial.print(F(" adv_fresh="));
  Serial.print(adv.seen && millis() - adv.lastMs < 5000);
  Serial.print(F(" gyro_rad_s x="));
  Serial.print(gyro.x(), 3);
  Serial.print(F(" y="));
  Serial.print(gyro.y(), 3);
  Serial.print(F(" z="));
  Serial.print(gyro.z(), 3);
  Serial.print(F(" cal sys="));
  Serial.print(sysCal);
  Serial.print(F(" gyro="));
  Serial.print(gyroCal);
  Serial.print(F(" accel="));
  Serial.print(accelCal);
  Serial.print(F(" mag="));
  Serial.println(magCal);
}

void startGpsSerial(int rxPin) {
  activeGpsRx = rxPin;
  gpsSerial.end();
  delay(20);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, activeGpsRx, GPS_TX);
  gpsProbeStartedMs = millis();
  gpsLastPassedChecksum = localGps.passedChecksum();
  Serial.print(F("GPS UART RX="));
  Serial.print(activeGpsRx);
  Serial.print(F(" TX=off baud="));
  Serial.println(GPS_BAUD);
}

void updateGps() {
  while (gpsSerial.available()) {
    gpsRawBytes++;
    localGps.encode(gpsSerial.read());
  }
}

void switchGpsRxIfNeeded() {
  if (localGps.passedChecksum() > 0 || activeGpsRx == GPS_RX_FALLBACK) {
    return;
  }
  if (millis() - gpsProbeStartedMs < GPS_RX_PROBE_MS) {
    return;
  }
  if (localGps.passedChecksum() == gpsLastPassedChecksum) {
    startGpsSerial(GPS_RX_FALLBACK);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("BNO055 north + ADV bearing display"));

  Wire.begin(I2C_SDA, I2C_SCL);
  startGpsSerial(GPS_RX);
  setupEspNow();

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(GC9A01A_BLACK);
  drawCenteredText("BNO055", 92, GC9A01A_WHITE, 2);
  drawCenteredText("starting", 118, GC9A01A_CYAN, 2);

  if (!bno.begin()) {
    Serial.println(F("BNO055 not detected. Check SDA=21 SCL=22, power, ground, and address 0x29/0x28."));
    tft.fillScreen(GC9A01A_BLACK);
    drawCenteredText("BNO055", 82, GC9A01A_RED, 2);
    drawCenteredText("not found", 112, GC9A01A_RED, 2);
    while (true) delay(1000);
  }

  bno.setExtCrystalUse(true);
  delay(500);
}

void loop() {
  updateGps();
  switchGpsRxIfNeeded();

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
  float headingDeg = normalizeDegrees(euler.x());

  uint8_t sysCal = 0, gyroCal = 0, accelCal = 0, magCal = 0;
  bno.getCalibration(&sysCal, &gyroCal, &accelCal, &magCal);

  uint32_t now = millis();

  // Transmit local GPS data over ESP-NOW once per second
  if (now - lastTransmitMs >= 1000) {
    lastTransmitMs = now;
    GpsNowPacket packet = {
      {'G', 'P', 'N', '1'},
      1,
      localGps.location.isValid() ? 1 : 0,
      sequenceNumber++,
      localGps.location.isValid() ? localGps.location.lat() : 0.0,
      localGps.location.isValid() ? localGps.location.lng() : 0.0,
      localGps.altitude.isValid() ? (float)localGps.altitude.meters() : 0.0f,
      localGps.hdop.isValid() ? (float)localGps.hdop.hdop() : 0.0f,
      localGps.satellites.isValid() ? (uint8_t)localGps.satellites.value() : 0,
      localGps.charsProcessed(),
      now
    };
    esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t*)&packet, sizeof(packet));
    if (result != ESP_OK) {
      Serial.print(F("COMPASS TX failed: "));
      Serial.println(result);
    } else {
      Serial.print(F("COMPASS TX seq="));
      Serial.print(packet.seq);
      Serial.print(F(" valid="));
      Serial.print(packet.valid);
      Serial.print(F(" sats="));
      Serial.println(packet.sats);
    }
  }

  if (now - lastSerialMs >= 250) {
    lastSerialMs = now;
    printStatus(headingDeg, sysCal, gyroCal, accelCal, magCal);
  }

  if (now - lastDrawMs >= 100) {
    lastDrawMs = now;
    drawCompass(headingDeg, sysCal, gyroCal, accelCal, magCal);
  }
}
