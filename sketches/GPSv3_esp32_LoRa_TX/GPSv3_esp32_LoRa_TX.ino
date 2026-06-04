#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>

const int RXPinGPS = 33;  // GPS TX -> ESP32 GPIO33
const int TXPinGPS = 32;  // GPS RX -> ESP32 GPIO32
const uint32_t GPSBaud = 9600;
const uint32_t TX_INTERVAL_MS = 1000;
const uint8_t ESPNOW_CHANNEL = 1;

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint16_t sequenceNumber = 0;
uint32_t lastTransmitMs = 0;
uint32_t txOkCount = 0;
uint32_t txFailCount = 0;
volatile bool lastSendDone = false;
volatile bool lastSendOk = false;

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

#if ESP_ARDUINO_VERSION_MAJOR >= 3
void onEspNowSent(const wifi_tx_info_t* info, esp_now_send_status_t status) {
#else
void onEspNowSent(const uint8_t* mac, esp_now_send_status_t status) {
#endif
  lastSendOk = status == ESP_NOW_SEND_SUCCESS;
  lastSendDone = true;
}

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.setSleep(false);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("BOOT espnow=FAIL"));
    return;
  }
  esp_now_register_send_cb(onEspNowSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("BOOT espnow_peer=FAIL"));
    return;
  }

  Serial.print(F("BOOT role=TX espnow=READY ch="));
  Serial.print(ESPNOW_CHANNEL);
  Serial.print(F(" MAC="));
  Serial.println(WiFi.macAddress());
}

GpsNowPacket buildPacket() {
  bool hasFix = gps.location.isValid();
  GpsNowPacket packet = {
    {'G', 'P', 'N', '1'},
    1,
    hasFix ? 1 : 0,
    sequenceNumber++,
    hasFix ? gps.location.lat() : 0.0,
    hasFix ? gps.location.lng() : 0.0,
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
  Serial.println(F("BOOT role=TX app=GPS_ESPNOW"));
  Serial.println(F("BOOT gps_uart=RX33_TX32_9600"));

  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
  setupEspNow();
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  const uint32_t now = millis();
  if (now - lastTransmitMs < TX_INTERVAL_MS) {
    delay(10);
    return;
  }
  lastTransmitMs = now;

  GpsNowPacket packet = buildPacket();
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t*)&packet, sizeof(packet));
  if (result == ESP_OK) {
    txOkCount++;
  } else {
    txFailCount++;
  }

  Serial.print(F("STATUS role=TX espnow="));
  Serial.print(result == ESP_OK ? F("QUEUED") : F("ERR"));
  Serial.print(F(" ok="));
  Serial.print(txOkCount);
  Serial.print(F(" fail="));
  Serial.print(txFailCount);
  Serial.print(F(" cb="));
  Serial.print(lastSendDone ? (lastSendOk ? F("OK") : F("FAIL")) : F("WAIT"));
  Serial.print(F(" seq="));
  Serial.print(packet.seq);
  Serial.print(F(" gps="));
  Serial.print(packet.valid ? F("fix") : F("no_fix"));
  Serial.print(F(" sats="));
  Serial.print(packet.sats);
  Serial.print(F(" lat="));
  if (packet.valid) {
    Serial.print(packet.lat, 6);
  } else Serial.print(F("--"));
  Serial.print(F(" lon="));
  if (packet.valid) Serial.print(packet.lon, 6); else Serial.print(F("--"));
  Serial.println();
}
