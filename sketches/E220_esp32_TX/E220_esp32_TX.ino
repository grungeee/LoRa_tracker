#include "Arduino.h"
#include <HardwareSerial.h>
#include "LoRa_E220.h"

#include <TinyGPSPlus.h>

// ====================<< LED Ring Settings >>===================
#include "LED_Ring.h" // (from a custom file)
// ------------- Funct ---------------
// Function to update the display after the fourth newline
void LEDR_COLOR(int ledIndex, int color[3], int delayTime);
void LEDcycle(int color[3]);


// ===================<< E220 LoRa Settings >>====================
#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

#define comChan 69 

// Define e220ttl object only if it's not defined
#ifndef E220_TTL_DEFINED
#define E220_TTL_DEFINED
LoRa_E220e220ttl e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
#endif



TinyGPSPlus gps; // Create an instance of the TinyGPSPlus object
HardwareSerial gpsSerial(1); // Create an instance of HardwareSerial object

const int RXPin = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 32; // TX pin oef ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate
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
  /////////
#include "Arduino.h"
#include "LoRa_E220.h"
#include "HardwareSerial.h"


// ====================<< LED Ring Settings >>===================
#include "LED_Ring.h" // (from a custom file)
// ------------- Funct ---------------
// Function to update the display after the fourth newline
void LEDR_COLOR(int ledIndex, int color[3], int delayTime);
void LEDcycle(int color[3]);


// ===================<< E220 LoRa Settings >>====================
#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too


#define MESSAGE_TYPE "HUMI"
#define ROOM "Kitchen"
#define PARAM "GPS"

struct MessageLatitude {
	char type[5];
	char message[8];
	byte value[4];
};

struct MessageLongtitude {
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

struct MessageTemperature {
	char type[5];
	char message[8];
	byte temperature[4];
};

struct MessageHumidity {
	char type[5];
	char message[8];
	byte humidity;
};





void setup() {

  Serial.begin(115200);
  delay(500);



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


  // === LED ===
// ------- LED Ring setup init ------- (from a file)
initializeLEDRing();
randomSeed(analogRead(0));

}

void loop() {

  
    // ============< LED Random >===============
  int randAnal = random(0,16);
  int randR = random(0,255);
  int randG = random(0,255);
  int randB = random(0,255);
  pixels.clear();
  pixels.setPixelColor(randAnal, pixels.Color(randR, randG, randG));
  delay(100);
  pixels.show();

  // ===================< E220 >=====================
	if (e220ttl.available() > 1) {


		// read the String message
		char type[5]; // first part of structure
		ResponseContainer rs = e220ttl.receiveInitialMessage(sizeof(type)); String typeStr = rs.data;

		// Is something goes wrong print error
		if (rs.status.code != 1) {
			Serial.println(rs.status.getResponseDescription());
		} else {
			Serial.println(typeStr);
			if (typeStr == "TEMP") {
				struct MessageTemperaturePartial {
					char message[8];
					byte temperature[4];
				};

				ResponseStructContainer rsc = e220ttl.receiveMessage( sizeof(MessageTemperaturePartial));
				struct MessageTemperaturePartial message = *(MessageTemperaturePartial*) rsc.data;

				Serial.println(*(float*)(message.temperature));
				Serial.println(message.message);
				rsc.close();
			} else if (typeStr == "HUMI") {
				struct MessageHumidityPartial {
					char message[8];
					byte humidity;
				};


				ResponseStructContainer rsc = e220ttl.receiveMessage(sizeof(MessageHumidityPartial));
				struct MessageHumidityPartial message = *(MessageHumidityPartial*) rsc.data;

				Serial.println(message.humidity);
				Serial.println(message.message);
				rsc.close();

			} else if (typeStr == "LATI") {
				struct MessageLatitudePartial {
					char message[8];
					byte value[4];
				};

				ResponseStructContainer rsc = e220ttl.receiveMessage(sizeof(MessageLatitudePartial));
				struct MessageLatitudePartial message = *(MessageLatitudePartial*) rsc.data;

				Serial.println(*(float*)(message.value));
				Serial.println(message.message);
				rsc.close();
			} else {
				Serial.println("Something goes wrong!!");

			}
		}
	}
 	if (Serial.available()) {
 		if (MESSAGE_TYPE == "HUMI") {
 			struct MessageHumidity message = { "HUMI", ROOM, 0 };
 			message.humidity = Serial.parseInt();

 	 		// Send message
 	 		ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message, sizeof(MessageHumidity));
 	 		// Check If there is some problem of succesfully send
 	 		Serial.println(rs.getResponseDescription());
 		} else {
 			struct MessageTemperature message = { "TEMP", ROOM, 0 };
 			*(float*)(message.temperature) = Serial.parseFloat();

 	 		// Send message
 	 		ResponseStatus rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, &message, sizeof(MessageTemperature));
 	 		// Check If there is some problem of succesfully send
 	 		Serial.println(rs.getResponseDescription());
 		}
 	}
}


how to fix this, i only get 				Serial.println("Something goes wrong!!");

is it possible to send messeges without the constructor interpretation of my messages
if use only a string it should print the value of the value but if it gets breakdwon and there is non of the existent. it retreives the data, from the buffer and if it is a normal string it should use one way of interpretation if another then another
  

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



      /*
    struct MessageLatitude messageLat = { "LATI", PARAM, 0};
   // *(float*)(messageLat.value) = gps.location.lat();
    *(float*)(messageLat.value) = 42.424212;
    ResponseStatus rsL = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, comChan, &messageLat, sizeof(MessageLatitude));

*/
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
