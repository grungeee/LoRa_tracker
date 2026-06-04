#include <Adafruit_NeoPixel.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

const uint8_t ESPNOW_CHANNEL = 1;
const int LED_RING_PIN = 21;
const int LEDS_NUM = 16;
const uint32_t STALE_TIMEOUT_MS = 5000;

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

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

volatile bool packetReady = false;
GpsEspNowPacket latestPacket = {};
uint8_t latestSender[6] = {};
uint32_t lastPacketMs = 0;
int currentLed = 0;

void printMac(const uint8_t *mac) {
  for (int i = 0; i < 6; i++) {
    if (i > 0) {
      Serial.print(":");
    }
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
  }
}

void showFreshPacket(bool fixValid) {
  pixels.clear();
  if (fixValid) {
    pixels.setPixelColor(currentLed, pixels.Color(0, 255, 0));
  } else {
    pixels.setPixelColor(currentLed, pixels.Color(255, 80, 0));
  }
  pixels.show();
  currentLed = (currentLed + 1) % LEDS_NUM;
}

void showStale() {
  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(30, 0, 0));
  pixels.show();
}

void onDataReceived(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  if (len != sizeof(GpsEspNowPacket)) {
    return;
  }

  GpsEspNowPacket incoming;
  memcpy(&incoming, data, sizeof(incoming));
  if (incoming.magic != 0x47505331 || incoming.version != 1) {
    return;
  }

  latestPacket = incoming;
  memcpy(latestSender, info->src_addr, sizeof(latestSender));
  lastPacketMs = millis();
  packetReady = true;
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

  esp_now_register_recv_cb(onDataReceived);
}

void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.clear();
  pixels.show();

  Serial.println("GPS ESP-NOW LED receiver started");
  Serial.print("Receiver MAC: ");
  Serial.println(WiFi.macAddress());

  setupEspNow();
  showStale();
}

void loop() {
  if (packetReady) {
    noInterrupts();
    GpsEspNowPacket packet = latestPacket;
    uint8_t sender[6];
    memcpy(sender, latestSender, sizeof(sender));
    packetReady = false;
    interrupts();

    Serial.print("RX #");
    Serial.print(packet.sequence);
    Serial.print(" from ");
    printMac(sender);
    Serial.print(" fix=");
    Serial.print(packet.fixValid ? "yes" : "no");
    Serial.print(" lat=");
    Serial.print(packet.latitude, 6);
    Serial.print(" lon=");
    Serial.print(packet.longitude, 6);
    Serial.print(" alt=");
    Serial.print(packet.altitudeMeters);
    Serial.print(" speed=");
    Serial.print(packet.speedKmph);
    Serial.print(" sats=");
    Serial.print(packet.satellites);
    Serial.print(" hdop=");
    Serial.println(packet.hdop);

    showFreshPacket(packet.fixValid);
  }

  if (millis() - lastPacketMs > STALE_TIMEOUT_MS) {
    static uint32_t lastStaleBlinkMs = 0;
    if (millis() - lastStaleBlinkMs > 1000) {
      lastStaleBlinkMs = millis();
      showStale();
    }
  }
}
