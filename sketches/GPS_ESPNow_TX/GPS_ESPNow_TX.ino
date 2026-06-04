#include <esp_now.h>
#include <esp_wifi.h>
#include <HardwareSerial.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

const int GPS_RX_PIN = 33; // ESP32 RX, connect to GPS TX
const int GPS_TX_PIN = 32; // ESP32 TX, connect to GPS RX
const uint32_t GPS_BAUD = 9600;
const uint8_t ESPNOW_CHANNEL = 1;
const uint32_t SEND_INTERVAL_MS = 1000;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

struct GpsEspNowPacket {
  uint32_t magic;
  uint16_t version;
  uint32_t sequence;
  double latitude;
  double longitude;
  float altitudeMeters;
  float speedKmph;
  uint32_t satellites;
  uint32_t hdop;
  bool fixValid;
  uint32_t ageMs;
};

uint32_t sequenceNumber = 0;
uint32_t lastSendMs = 0;

void onDataSent(const esp_now_send_info_t *txInfo, esp_now_send_status_t status) {
  (void)txInfo;
  Serial.print("ESP-NOW send: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "ok" : "failed");
}

void setupEspNow() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    while (true) {
      delay(1000);
    }
  }

  esp_now_register_send_cb(onDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, sizeof(broadcastAddress));
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("ESP-NOW peer add failed");
    while (true) {
      delay(1000);
    }
  }
}

void sendGpsPacket() {
  GpsEspNowPacket packet = {};
  packet.magic = 0x47505331; // "GPS1"
  packet.version = 1;
  packet.sequence = ++sequenceNumber;
  packet.fixValid = gps.location.isValid();
  packet.ageMs = gps.location.age();

  if (packet.fixValid) {
    packet.latitude = gps.location.lat();
    packet.longitude = gps.location.lng();
    packet.altitudeMeters = gps.altitude.isValid() ? gps.altitude.meters() : 0.0f;
    packet.speedKmph = gps.speed.isValid() ? gps.speed.kmph() : 0.0f;
    packet.satellites = gps.satellites.isValid() ? gps.satellites.value() : 0;
    packet.hdop = gps.hdop.isValid() ? gps.hdop.value() : 0;
  }

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&packet, sizeof(packet));

  Serial.print("TX #");
  Serial.print(packet.sequence);
  Serial.print(" fix=");
  Serial.print(packet.fixValid ? "yes" : "no");
  Serial.print(" lat=");
  Serial.print(packet.latitude, 6);
  Serial.print(" lon=");
  Serial.print(packet.longitude, 6);
  Serial.print(" sats=");
  Serial.print(packet.satellites);
  Serial.print(" result=");
  Serial.println(result == ESP_OK ? "queued" : "error");
}

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  Serial.println("GPS ESP-NOW transmitter started");
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());

  setupEspNow();
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (millis() - lastSendMs >= SEND_INTERVAL_MS) {
    lastSendMs = millis();
    sendGpsPacket();
  }
}
