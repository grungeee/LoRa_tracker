#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// Create an instance of the TinyGPSPlus object
TinyGPSPlus gps;

// Create an instance of HardwareSerial object
HardwareSerial mySerial(1);

// Pin definitions
const int RXPin = 16; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 17; // TX pin of ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate

void setup()
{
  // Start the serial communication with the GPS module
  mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  // Start the serial communication with the computer
  Serial.begin(115200);

  Serial.println("GPS Module testing");
}

void loop()
{
  // Read data from GPS module
  while (mySerial.available() > 0)
  {
    // Feed the data into the TinyGPSPlus object
    gps.encode(mySerial.read());

    // Display information from the GPS module
    if (gps.location.isUpdated())
    {
      Serial.print("Latitude: ");
      Serial.println(gps.location.lat(), 6);
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      Serial.print("Altitude: ");
      Serial.println(gps.altitude.meters());
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      Serial.print("HDOP: ");
      Serial.println(gps.hdop.value());
    }
  }
}
