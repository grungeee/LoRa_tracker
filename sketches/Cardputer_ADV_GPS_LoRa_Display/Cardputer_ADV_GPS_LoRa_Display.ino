#include <Arduino.h>
#include <M5Cardputer.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

const int GPS_RX = 15;  // Cap GPS_TX -> Cardputer-Adv GPIO15
const int GPS_TX = 13;  // Cardputer-Adv GPIO13 -> Cap GPS_RX
const uint32_t GPS_BAUD = 115200;

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

struct RemoteState {
  bool seen = false;
  GpsNowPacket packet = {};
  uint32_t lastMs = 0;
  int8_t rssi = 0;
};

RemoteState remote;
uint32_t lastDrawMs = 0;
uint32_t lastTransmitMs = 0;
uint16_t sequenceNumber = 0;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

  remote.packet = packet;
  remote.seen = true;
  remote.lastMs = millis();
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  remote.rssi = info && info->rx_ctrl ? info->rx_ctrl->rssi : 0;
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

  Serial.print(F("ESP-NOW AP channel: "));
  Serial.print(WiFi.channel());
  Serial.print(F(", MAC: "));
  Serial.println(WiFi.softAPmacAddress());
}

GpsNowPacket buildLocalPacket() {
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
    millis()
  };
  return packet;
}

String coord(bool valid, double value) {
  return valid ? String(value, 5) : "--";
}

String altText(bool valid, double value) {
  return valid ? String(value, 0) + "m" : "--";
}

String distanceText() {
  if (!remote.seen || !remote.packet.valid || !localGps.location.isValid()) {
    return "--";
  }

  double horizontal = TinyGPSPlus::distanceBetween(
    localGps.location.lat(),
    localGps.location.lng(),
    remote.packet.lat,
    remote.packet.lon
  );
  double localAlt = localGps.altitude.isValid() ? localGps.altitude.meters() : 0.0;
  double dAlt = remote.packet.alt - localAlt;
  double distance = sqrt(horizontal * horizontal + dAlt * dAlt);

  if (distance >= 1000.0) {
    return String(distance / 1000.0, 2) + "km";
  }
  return String(distance, 0) + "m";
}

void labelValue(const char* label, const String& value, int x, int y, int valueWidth, uint16_t color = TFT_WHITE) {
  auto& d = M5Cardputer.Display;
  d.setTextColor(TFT_DARKGREY, TFT_BLACK);
  d.drawString(label, x, y);
  d.fillRect(x + 34, y, valueWidth, 10, TFT_BLACK);
  d.setTextColor(color, TFT_BLACK);
  d.drawString(value, x + 34, y);
}

void drawUi() {
  auto& d = M5Cardputer.Display;
  d.fillScreen(TFT_BLACK);
  d.setTextFont(&fonts::Font0);
  d.setTextSize(1);
  d.setTextDatum(top_left);

  bool remoteFresh = remote.seen && millis() - remote.lastMs < 5000;
  bool remoteValid = remote.seen && remote.packet.valid;
  bool localValid = localGps.location.isValid();

  d.setTextColor(TFT_CYAN, TFT_BLACK);
  d.drawString("GPS ESP-NOW", 4, 2);
  d.setTextColor(remoteFresh ? TFT_GREEN : TFT_ORANGE, TFT_BLACK);
  d.drawString(remoteFresh ? "RX" : "WAIT", 190, 2);

  d.drawFastHLine(0, 15, 240, TFT_DARKGREY);

  d.setTextColor(TFT_GREEN, TFT_BLACK);
  d.drawString("REMOTE", 4, 19);
  labelValue("seq", remote.seen ? String(remote.packet.seq) : "--", 70, 19, 50, remoteFresh ? TFT_WHITE : TFT_ORANGE);
  labelValue("sat", remote.seen ? String(remote.packet.sats) : "--", 142, 19, 38);
  labelValue("lat", coord(remoteValid, remote.packet.lat), 4, 34, 96);
  labelValue("lon", coord(remoteValid, remote.packet.lon), 4, 47, 96);
  labelValue("alt", altText(remoteValid, remote.packet.alt), 142, 34, 52);
  labelValue("hdp", remote.seen ? String(remote.packet.hdop, 1) : "--", 142, 47, 52);

  d.drawFastHLine(0, 62, 240, TFT_DARKGREY);

  d.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  d.drawString("LOCAL", 4, 66);
  labelValue("sat", localGps.satellites.isValid() ? String(localGps.satellites.value()) : "--", 70, 66, 38, localValid ? TFT_WHITE : TFT_ORANGE);
  labelValue("lat", coord(localValid, localGps.location.lat()), 4, 81, 96);
  labelValue("lon", coord(localValid, localGps.location.lng()), 4, 94, 96);
  labelValue("alt", altText(localGps.altitude.isValid(), localGps.altitude.meters()), 142, 81, 52);
  labelValue("hdp", localGps.hdop.isValid() ? String(localGps.hdop.hdop(), 1) : "--", 142, 94, 52);

  d.fillRect(0, 113, 240, 22, TFT_NAVY);
  d.setTextColor(TFT_WHITE, TFT_NAVY);
  d.drawString("D " + distanceText(), 4, 119);
  d.drawString("RSSI " + String(remote.rssi), 82, 119);
  d.drawString(remoteFresh ? "fresh" : "stale", 170, 119);
}

void setup() {
  Serial.begin(115200);

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setBrightness(120);
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextColor(TFT_WHITE, TFT_BLACK);
  M5Cardputer.Display.drawString("Starting ESP-NOW GPS...", 4, 4);

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
  setupEspNow();
}

void loop() {
  M5Cardputer.update();

  while (gpsSerial.available()) {
    localGps.encode(gpsSerial.read());
  }

  if (millis() - lastTransmitMs >= 1000) {
    lastTransmitMs = millis();
    GpsNowPacket packet = buildLocalPacket();
    esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t*)&packet, sizeof(packet));
    if (result != ESP_OK) {
      Serial.print(F("ADV TX failed: "));
      Serial.println(result);
    } else {
      Serial.print(F("ADV TX seq="));
      Serial.print(packet.seq);
      Serial.print(F(" valid="));
      Serial.print(packet.valid);
      Serial.print(F(" sats="));
      Serial.println(packet.sats);
    }
  }

  if (millis() - lastDrawMs >= 250) {
    lastDrawMs = millis();
    drawUi();
  }
}
