#include <HardwareSerial.h>
#include <TinyGPSPlus.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

const int GPS_RX_PIN = 33; // ESP32 RX, connect to GPS TX
const int GPS_TX_PIN = 32; // ESP32 TX, connect to GPS RX
const uint32_t GPS_BAUD = 9600;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  Serial.println("GPS serial monitor started");
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Speed km/h: ");
    Serial.println(gps.speed.kmph());
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.print("HDOP: ");
    Serial.println(gps.hdop.value());
    Serial.println();
  }
}
