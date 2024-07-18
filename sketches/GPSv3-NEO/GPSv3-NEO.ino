#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Create a TinyGPS++ object
TinyGPSPlus gps;

// Define the serial pins for GPS
SoftwareSerial ss(2, 3); // RX, TX

void setup() {
  // Start the serial communication with the GPS module
  ss.begin(9600);
  
  // Start the serial communication with the computer
  Serial.begin(9600);
  Serial.println(F("GPS Module is initializing"));
}

void loop() {
  // Read data from the GPS module
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
  
  // If a new location is available, display it
  if (gps.location.isUpdated()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    Serial.print("Speed: ");
    Serial.println(gps.speed.kmph());
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
    Serial.println();
  }
}
