/*
 * EBYTE LoRa E220
 *
 * Send a string message to all devices of the Channel CHAN 23
 *
 * Write a string on serial monitor or reset to send the string to all device on channel 23.
 *
 * Send a fixed message, you must check that the transmitter and receiver have different
 * ADDL or ADDH, check the configuration down
 *
 * For the test you can use
 *  - BROADCAST MESSAGE 1
 *  - BROADCAST MESSAGE 2
 *  - BROADCAST MESSAGE 3
 *
 * Pai attention e220 support RSSI, if you want use that functionality you must enable RSSI on configuration
 * configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
 * and uncomment #define ENABLE_RSSI true in this sketch
 *
 * https://www.mischianti.org
 *
 * E220		  ----- WeMos D1 mini	----- esp32			----- Arduino Nano 33 IoT	----- Arduino MKR	----- Raspberry Pi Pico   ----- stm32               ----- ArduinoUNO
 * M0         ----- D7 (or GND) 	----- 19 (or GND) 	----- 4 (or GND) 			----- 2 (or GND) 	----- 10 (or GND)	      ----- PB0 (or GND)        ----- 7 Volt div (or GND)
 * M1         ----- D6 (or GND) 	----- 21 (or GND) 	----- 6 (or GND) 			----- 4 (or GND) 	----- 11 (or GND)	      ----- PB10 (or GND)       ----- 6 Volt div (or GND)
 * TX         ----- D3 (PullUP)		----- TX2 (PullUP)	----- TX1 (PullUP)			----- 14 (PullUP)	----- 8 (PullUP)	      ----- PA2 TX2 (PullUP)    ----- 4 (PullUP)
 * RX         ----- D4 (PullUP)		----- RX2 (PullUP)	----- RX1 (PullUP)			----- 13 (PullUP)	----- 9 (PullUP)	      ----- PA3 RX2 (PullUP)    ----- 5 Volt div (PullUP)
 * AUX        ----- D5 (PullUP)		----- 18  (PullUP)	----- 2  (PullUP)			----- 0  (PullUP)	----- 2  (PullUP)	      ----- PA0  (PullUP)       ----- 3 (PullUP)
 * VCC        ----- 3.3v/5v			----- 3.3v/5v		----- 3.3v/5v				----- 3.3v/5v		----- 3.3v/5v		      ----- 3.3v/5v             ----- 3.3v/5v
 * GND        ----- GND				----- GND			----- GND					----- GND			----- GND			      ----- GND                 ----- GND
 *
 */

// If you want use RSSI uncomment //#define ENABLE_RSSI true
// and use relative configuration with RSSI enabled
//#define ENABLE_RSSI true

// /==========================================/ START /===================================/
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21
#define LEDS_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};
  int chill[3] = {40,8,40};
  int pink[3] = {125,10,10};
  int yellow[3] = {255, 255, 0};
  int dim_yellow[3] = {20, 20, 0};
  int dim_white[3] = {20, 20, 20};
  int dim_blue[3] = {0, 0, 20};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
  pixels.clear();
  pixels.show();
}
// -----------------------------------------
void LEDcycle (int color[3]){
  for (int i=0; i<LEDS_NUM; i++) {
    LEDR_COLOR(i,color,100);
  }
}
// |================================================|



// ========================================================================================
// Create an instance of the TinyGPSPlus object
TinyGPSPlus gps;

// Create an instance of HardwareSerial object
HardwareSerial mySerial(1);

const int RXPin = 33; // RX pin of ESP32 connected to TX of GPS module
const int TXPin = 32; // TX pin of ESP32 connected to RX of GPS module
const uint32_t GPSBaud = 9600; // GPS module baud rate
int chan = 23;
String message; //empty string
// ========================================================================================

// ---------- Arduino pins --------------
//LoRa_E220 e220ttl(4, 5, 3, 7, 6); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX AUX M0 M1
//LoRa_E220 e220ttl(4, 5); // Config without connect AUX and M0 M1

//#include <SoftwareSerial.h>
//SoftwareSerial mySerial(4, 5); // Arduino RX <-- e220 TX, Arduino TX --> e220 RX
//LoRa_E220 e220ttl(&mySerial, 3, 7, 6); // AUX M0 M1
// -------------------------------------

// ---------- esp32 pins --------------
/*
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example#include "Arduino.h"
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
#define M1_PIN 15 // 21 in example
*/
#define TX_PIN 21 //  TX2  in example
#define RX_PIN 20 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 3 // 21 in example
// Just drop the mohterfucking M0 and M1 pins
// --> GND this bitch

// LoRa_E220 e220ttl(&Serial2, 15, 21, 19); //  RX AUX M0 M1

//LoRa_E220 e220ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e220 TX, esp32 TX --> e220 RX AUX M0 M1
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

// =============================<< SETUP >======================================
void setup() {

// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
pixels.begin(); 
pixels.clear();
pixels.show();
// |=============================================|

  // Start the serial communication with the GPS module
  mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
  // Start the serial communication with the computer
  Serial.begin(115200);

  Serial.println("GPS Module testing");
    /////////////////
  delay(500);

  // Startup all pins and UART
  e220ttl.begin();
    // ------------------------ config ----------------------
	ResponseStructContainer c;
	c = e220ttl.getConfiguration();
	Configuration configuration = *(Configuration*) c.data;
   // ResponseStatus rs = e220ttl.setConfiguration(configuration);
	configuration.CHAN = 23; // Communication channel
	configuration.ADDL = 0x03;  // First part of address
	configuration.ADDH = 0x00; // Second part
    //configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    // ----------------------------------------------


  Serial.println("Hi, I'm going to send message!");

  // Send message
 //ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
 ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,"Hi, I'm C3!");

    LEDR_COLOR(8,chill, 1000);
    LEDR_COLOR(8,pink, 1000);
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());
}

/////

void loop() {
    LEDR_COLOR(0,dim_blue,1000);
    delay(1000);

  // Read data from GPS module
  if (mySerial.available() > 0)
  {
    // Feed the data into the TinyGPSPlus object
    gps.encode(mySerial.read());

    // Display information from the GPS module
    if (gps.location.isUpdated())
    {
    message = "Latitude: " + String(gps.location.lat(), 6) + "\n" + "Longitude: " + String(gps.location.lng(), 6) + "\n" "Altitude: " + String(gps.altitude.meters()) + "\n" + "Satellites: " + String(gps.satellites.value()) + "\n" + "HDOP: " + String(gps.hdop.value());
    //message = String(gps.location.lat(), 6);
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(chan, message);
            /*
        WiFi:
        Latitude: 48.187563
        Longitude: 15.617960
        Altitude: 304.40            /*

     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lng(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.location.lat(), 6));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.altitude.meters()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.satellites.value()));
     ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23,String(gps.hdop.value()));
            */
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
	// If something available
  if (e220ttl.available()>1) {
	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220ttl.receiveMessageRSSI();
#else
	ResponseContainer rc = e220ttl.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, input);
	  // Check If there is some problem or? succesfully send
	  Serial.println(rs.getResponseDescription());
        LEDR_COLOR(8, green, 1000); 
  }
}


//	----------------------- DEFAULT TRANSPARENT WITH RSSI -----------------------
//	configuration.ADDL = 0x03;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
// ResponseStatus rs = e220ttl.setConfiguration(configuration);
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
//	----------------------- FIXED SENDER -----------------------
//	configuration.ADDL = 0x02;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
//
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
//
//	----------------------- FIXED RECEIVER -----------------------
//	configuration.ADDL = 0x03;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
//
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01LCD Display GC9A01AAAAAAAAAAAAAAA;
