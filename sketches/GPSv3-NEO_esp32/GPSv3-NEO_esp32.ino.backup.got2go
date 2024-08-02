#include "Arduino.h"
#include "LoRa_E220.h"

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too




TinyGPSPlus gps; // Create an instance of the TinyGPSPlus object
HardwareSerial gpsSerial(1); // Create an instance of HardwareSerial object

const int RXPin = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 32; // TX pin oef ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate
int comChan = 69;
// Pin definitions
//const int RXPin = 16; // RX pin of ESP32 connected to TX of GPS module
//const int TXPin = 17; // TX pin of ESP32 connected to RX of GPS module
//const int RXPin = 3; // RX pin of ESP32 connected to TX of GPS module
//const int TXPin = 1; // TX pin of ESP32 connected to RX of GPS module
//const int RXPin = 10; // RX pin of ESP32 connected to TX of GPS module
//const int TXPin = 9; // TX pin of ESP32 connected to RX of GPS module

#define MESSAGE_TYPE "HUMI"
#define ROOM "Kitchen"
#define PARAM "GPS"



struct MessageHumidity {
	char type[5];
	char message[8];
	byte humidity;
};

struct MessageTemperature {
	char type[5];
	char message[8];
	byte temperature[4];
};

struct MessageLatitude {
	char type[5];
	char message[8];
	byte value[4];
};

struct MessageLongitude {
	char type[5];
	char message[8];
	byte value[4];
};

struct MessageAltitude {
	char type[5];
	char message[8];
	byte value[4];
};

struct MessageSatellites {
	char type[5];
	char message[8];
	byte value;
};


void setup()
{
  // Start the serial communication with the GPS module
  gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
// Start the serial communication with the computer
  Serial.begin(115200);

  Serial.println("GPS Module testing");


//////////////////// ===
  e220ttl.begin();
  Serial.println("Start receiving!");

ResponseStructContainer c;
c = e220ttl.getConfiguration();
Configuration configuration = *(Configuration*) c.data;
 
configuration.ADDL = BROADCAST_ADDRESS; // OR: 0x03
configuration.ADDH = BROADCAST_ADDRESS; // OR: 0x00
 
configuration.CHAN = 69;
 
configuration.SPED.uartBaudRate = UART_BPS_9600;
configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
configuration.SPED.uartParity = MODE_00_8N1;
 
configuration.OPTION.subPacketSetting = SPS_200_00;
configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
configuration.OPTION.transmissionPower = POWER_22;
 
configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;

//configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // FT_FIXED_TRANSMISSION; //repeater mode off
configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
         
e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
c.close();

}

void loop()
{

  // Read data from GPS module
  if (gpsSerial.available() > 0)
  {
    // Feed the data into the TinyGPSPlus object
    gps.encode(gpsSerial.read());

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

            /*
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(comChan,"Latitude" + String(gps.location.lat(), 6));
     rs = e220ttl.sendBroadcastFixedMessage(comChan,"Latitude: " + String(gps.location.lat(), 6));
     rs = e220ttl.sendBroadcastFixedMessage(comChan,"Longtitude: " + String(gps.location.lng(), 6));
     rs = e220ttl.sendBroadcastFixedMessage(comChan,"Altitude: " + String(gps.altitude.meters()));
     rs = e220ttl.sendBroadcastFixedMessage(comChan,"Satellites: " + String(gps.satellites.value()));
     rs = e220ttl.sendBroadcastFixedMessage(comChan,"HDOP: " + String(gps.hdop.value(), 6));
     */


  //struct	Message message = {"lat", PARAM, 270};

//	struct MessageHumidity messageH = { "HUMI", ROOM, 80 };
	// Send message
//	ResponseStatus rsH= e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 69, &messageH, sizeof(MessageHumidity));

   struct MessageTemperature messageT = { "TEMP", ROOM, 19.2 };
    *(float*)(messageT.temperature) = 19.2;
    ResponseStatus rsT = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageT, sizeof(MessageTemperature));



    struct MessageLatitude messageLat = { "LATI", PARAM, 0};
   // *(float*)(messageLat.value) = gps.location.lat();
    *(float*)(messageLat.value) = 42.424212;
    ResponseStatus rsL = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageLat, sizeof(MessageLatitude));

/*
    MessageLongitude messageLng = { "LONG", PARAM, 0 };
    *(float*)(messageLng.value) = gps.location.lng();  // Cast and assign longitude to byte array

    ResponseStatus rsLo = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageLng, sizeof(MessageLongitude));


    MessageAltitude messageAlt = { "ALTI", PARAM, 0 };
    *(float*)(messageAlt.value) = gps.altitude.meters();  // Cast and assign altitude to byte array
    ResponseStatus rsA = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageAlt, sizeof(MessageAltitude));


    MessageSatellites messageSat = { "SATS", PARAM, 0};
    ResponseStatus rsS = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageSat, sizeof(MessageSatellites));

            */



    }
  }
}
