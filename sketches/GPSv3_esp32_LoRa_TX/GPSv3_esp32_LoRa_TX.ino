#include <Arduino.h>
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <M5Cardputer.h>

// ADV / Cardputer-Adv GPS sender.
// Reads GPS on GPIO15/13, sends packets over ESP-NOW, and shows status on screen.

static const int GPS_RX_PIN = 15;  // Cap GPS_TX -> Cardputer-Adv GPIO15
static const int GPS_TX_PIN = 13;  // Cardputer-Adv GPIO13 -> Cap GPS_RX
static const uint32_t GPS_BAUD = 115200;
static const uint8_t ESPNOW_CHANNEL = 1;
static const uint32_t SEND_INTERVAL_MS = 1000;
static const char DEVICE_NAME[] = "ADV";
static const uint8_t DEVICE_NAME_LEN = 12;

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint16_t sequenceNumber = 0;
uint32_t lastSendMs = 0;
volatile bool lastSendDone = false;
volatile bool lastSendOk = false;
bool espNowReady = false;
bool txQueued = false;
uint32_t txOkCount = 0;
uint32_t txFailCount = 0;

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
  WiFi.disconnect(true, true);
  WiFi.setSleep(false);
  esp_wifi_set_ps(WIFI_PS_NONE);
  esp_wifi_set_channel(ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE);

  if (esp_now_init() != ESP_OK) {
    Serial.println(F("BOOT espnow=FAIL"));
    return;
  }
  espNowReady = true;

  esp_now_register_send_cb(onEspNowSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = ESPNOW_CHANNEL;
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println(F("BOOT espnow_peer=FAIL"));
    espNowReady = false;
    return;
  }

  Serial.print(F("BOOT role=TX espnow=READY ch="));
  Serial.print(ESPNOW_CHANNEL);
  Serial.print(F(" mac="));
  Serial.println(WiFi.macAddress());
}

GpsNowPacket buildPacket() {
  const bool hasFix = gps.location.isValid();
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
    millis(),
    ""
  };
  strncpy(packet.name, DEVICE_NAME, DEVICE_NAME_LEN);
  return packet;
}

void drawStatus(const GpsNowPacket& packet) {
  auto& d = M5Cardputer.Display;
  const bool espOk = espNowReady && txQueued && lastSendDone && lastSendOk;
  const char* espText = !espNowReady ? "FAIL" : (espOk ? "TX OK" : (txQueued ? "SENT" : "WAIT"));
  const uint16_t espColor = espOk ? TFT_GREEN : (espNowReady ? TFT_YELLOW : TFT_RED);
  const uint16_t gpsColor = packet.valid ? TFT_GREEN : TFT_ORANGE;

  d.fillScreen(TFT_BLACK);
  d.setTextWrap(false);
  d.setTextSize(2);
  d.setCursor(0, 0);

  d.setTextColor(TFT_CYAN, TFT_BLACK);
  d.println("ADV GPS");

  d.setTextColor(espColor, TFT_BLACK);
  d.print("ESP ");
  d.println(espText);

  d.setTextColor(gpsColor, TFT_BLACK);
  d.print("GPS ");
  d.println(packet.valid ? "FIX" : "NO FIX");

  d.setTextColor(TFT_YELLOW, TFT_BLACK);
  d.print("SAT ");
  d.println(packet.sats);

  d.setTextColor(TFT_WHITE, TFT_BLACK);
  d.print("CHR ");
  d.println(packet.gpsChars);

  d.setTextColor(TFT_MAGENTA, TFT_BLACK);
  d.print("LAT ");
  if (packet.valid) {
    d.println(packet.lat, 5);
  } else {
    d.println("--");
  }

  d.setTextColor(TFT_SKYBLUE, TFT_BLACK);
  d.print("LON ");
  if (packet.valid) {
    d.println(packet.lon, 5);
  } else {
    d.println("--");
  }

  d.setTextColor(TFT_ORANGE, TFT_BLACK);
  d.print("ALT ");
  d.print(packet.alt, 1);
  d.println("m");

  d.setTextColor(TFT_WHITE, TFT_BLACK);
  d.print("OK ");
  d.print(txOkCount);
  d.print(" F ");
  d.println(txFailCount);
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println(F("BOOT role=TX app=GPS_ESPNOW"));
  Serial.println(F("BOOT gps_uart=G15_RX_G13_TX_115200"));

  auto cfg = M5.config();
  M5Cardputer.begin(cfg);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setBrightness(200);
  M5Cardputer.Display.fillScreen(TFT_BLACK);
  M5Cardputer.Display.setTextColor(TFT_CYAN, TFT_BLACK);
  M5Cardputer.Display.setTextSize(2);
  M5Cardputer.Display.setCursor(0, 0);
  M5Cardputer.Display.println("Booting");

  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);
  setupEspNow();
}

void loop() {
  M5Cardputer.update();

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  const uint32_t now = millis();
  if (now - lastSendMs < SEND_INTERVAL_MS) {
    delay(10);
    return;
  }
  lastSendMs = now;

  GpsNowPacket packet = buildPacket();
  lastSendDone = false;
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t*)&packet, sizeof(packet));
  txQueued = result == ESP_OK;
  if (txQueued) {
    txOkCount++;
  } else {
    txFailCount++;
  }
  delay(20);

  Serial.print(F("TX seq="));
  Serial.print(packet.seq);
  Serial.print(F(" esp="));
  Serial.print(result == ESP_OK ? F("QUEUED") : F("ERR"));
  Serial.print(F(" cb="));
  Serial.print(lastSendDone ? (lastSendOk ? F("OK") : F("FAIL")) : F("WAIT"));
  Serial.print(F(" gps="));
  Serial.print(packet.valid ? F("fix") : F("no_fix"));
  Serial.print(F(" sats="));
  Serial.print(packet.sats);
  Serial.print(F(" lat="));
  if (packet.valid) {
    Serial.print(packet.lat, 6);
  } else {
    Serial.print(F("--"));
  }
  Serial.print(F(" lon="));
  if (packet.valid) {
    Serial.print(packet.lon, 6);
  } else {
    Serial.print(F("--"));
  }
  Serial.print(F(" alt="));
  Serial.print(packet.alt, 1);
  Serial.print(F(" hdop="));
  Serial.println(packet.hdop, 1);

  drawStatus(packet);
}
