#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>

const int RXPinGPS = 33;  // GPS TX -> ESP32 GPIO33
const int TXPinGPS = 32;  // GPS RX -> ESP32 GPIO32
const uint32_t GPSBaud = 9600;
const uint32_t TX_INTERVAL_MS = 1000;

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint16_t sequenceNumber = 0;
uint32_t lastTransmitMs = 0;

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

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.setSleep(false);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("ESP-NOW init failed"));
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("ESP-NOW add broadcast peer failed"));
    return;
  }

  Serial.print(F("ESP-NOW TX MAC: "));
  Serial.println(WiFi.macAddress());
}

GpsNowPacket buildPacket() {
  GpsNowPacket packet = {
    {'G', 'P', 'N', '1'},
    1,
    gps.location.isValid() ? 1 : 0,
    sequenceNumber++,
    gps.location.isValid() ? gps.location.lat() : 0.0,
    gps.location.isValid() ? gps.location.lng() : 0.0,
    gps.altitude.isValid() ? (float)gps.altitude.meters() : 0.0f,
    gps.hdop.isValid() ? (float)gps.hdop.hdop() : 0.0f,
    gps.satellites.isValid() ? (uint8_t)gps.satellites.value() : 0,
    gps.charsProcessed(),
    millis()
  };
  return packet;
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println(F("ESP32 GPS -> ESP-NOW TX"));
  Serial.println(F("GPS UART: RX=33 TX=32 baud=9600"));

  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
  setupEspNow();
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  const uint32_t now = millis();
  if (now - lastTransmitMs < TX_INTERVAL_MS) {
    return;
  }
  lastTransmitMs = now;

  GpsNowPacket packet = buildPacket();
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t*)&packet, sizeof(packet));

  Serial.print(F("TX seq="));
  Serial.print(packet.seq);
  Serial.print(F(" result="));
  Serial.print(result == ESP_OK ? F("OK") : F("ERR"));
  Serial.print(F(" valid="));
  Serial.print(packet.valid);
  Serial.print(F(" sats="));
  Serial.print(packet.sats);
  Serial.print(F(" chars="));
  Serial.print(packet.gpsChars);
  if (packet.valid) {
    Serial.print(F(" lat="));
    Serial.print(packet.lat, 6);
    Serial.print(F(" lon="));
    Serial.print(packet.lon, 6);
  }
  Serial.println();
}
