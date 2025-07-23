// =========================<< GPS >>================================
#include "GPS_Module.h"


TinyGPSPlus gps; // Create an instance of the TinyGPSPlus object
HardwareSerial gpsSerial(1); // Create an instance of HardwareSerial object
const int RXPin = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 32; // TX pin of ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate
//String message; //empty string

void initGPS() {
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  Serial.println("GPS Module initialized");
}

bool updateGPS() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      return gps.location.isUpdated();
    }
  }
  return false;
}

String getGPSData() {
  if (gps.location.isValid()) {
    return "Latitude: " + String(gps.location.lat(), 6) + "\n" +
           "Longitude: " + String(gps.location.lng(), 6) + "\n" +
           "Altitude: " + String(gps.altitude.meters()) + "\n" +
           "Satellites: " + String(gps.satellites.value()) + "\n" +
           "HDOP: " + String(gps.hdop.value());
  } else {
    return "NO GPS DATA";
  }
}


      /*
        WiFi:
        Latitude: 48.187563
        Longitude: 15.617960
        Altitude: 304.40           
      */

    /*message = "Latitude: " + String(gps.location.lat(), 6) + "\n" + "Longitude: " + String(gps.location.lng(), 6) + "\n" "Altitude: " + String(gps.altitude.meters()) + "\n" + "Satellites: " + String(gps.satellites.value()) + "\n" + "HDOP: " + String(gps.hdop.value());*/

        /*

     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lng(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lat(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.altitude.meters()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.satellites.value()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.hdop.value()));
            */
