#include <Arduino.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Arduino.h>
// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h> #endif
#endif

#define LED_RING_PIN 16
#define LEDS_NUM 16
#define LEDindex 0

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

// ------------| Function |-------------------
  int red[3] = {255,0,0};
  int green[3] = {0,255,0};
  int blue[3] = {0,0,255};
  int purple[3] = {255,0,255};
  int chill[3] = {40,8,40};

void LEDR_COLOR (int ledIndex, int color[3],int delayTime){
  pixels.clear();
  pixels.show();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();

  delay(delayTime);
  pixels.show();
}
// -----------------------------------------

// |================================================|

// Message counter
//int msgCount = 0;

const int buttonPin= 8; // Button connected to D8

//////////////////////////////////////////////////////////////////////////

/*
SCL: A5
SDA: A4

LED Ring:
N: 
- X: 0
- Y: 0




E 
S
W

*/

MPU6050 mpu;

void setup() {
// |=============< LED RING SETUP >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);#include "Arduino.h"
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
#endif
  pixels.begin(); 
  pixels.clear();#include "Arduino.h"
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
  pixels.show();

  Serial.begin(115200);
  Wire.begin();
  mpu.initialize();
  if (mpu.testConnection()) {
      Serial.println("MPU6050 connection successful");
  } else {
      Serial.println("MPU6050 connection failed");
  }

// |=============================================|

// |===================< HMS >===================|
  pinMode(buttonPin, INPUT_PULLUP);
// |=============================================|
}

/*
Axis Drirection:
x ccw
y ccw
z cw
*/

void loop() {


    int16_t ax, ay, az;
    int16_t gx, gy, gz;

    mpu.getAcceleration(&ax, &ay, &az);
    mpu.getRotation(&gx, &gy, &gz);

    //float s = 161.0 * 2; // sensitivity scale factor
    float s = 1; // sensitivity scale factor
    float gxs = gx / s;
    float gys = gy / s;
    float gzs = gz / s;

  // ----------- LED 2 -------------
  float segment = 360.0 / LEDS_NUM;
  float GX_loop = gxs * 1;
  float x = GX_loop / segment;

  if (x > 0) {
    if ((int(x) - x) * (-1) < int(x) + 1 - x) {
      LEDR_COLOR(int(x),purple, 500);
    } else {
      LEDR_COLOR(int(x + 1),purple, 500);
    }
  } else {
    GX_loop = GX_loop * (-1);
    x = GX_loop / 22.5;
    if ((int(x) - x) * (-1) < int(x) + 1 - x) {
      LEDR_COLOR(int(15 - x),red, 500);
    } else {
      LEDR_COLOR(int(15 - (x + 1)),red, 500);
    }
  }



  // ----------- LED -------------
    /*
  float GX_loop = gxs;

  GX_loop = GX_loop < 0 ? 360 + GX_loop : GX_loop;

  for (int i=0;i<LEDS_NUM;i++){
   if  (i * segment <= GX_loop && GX_loop <= (i+1) * segment){
     LEDR_COLOR(i,purple, 0);
   }
 }
    */
// -----------------------------
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);

    Serial.println("-----------------\n");
    Serial.print(gxs); Serial.print("\t");
    Serial.print(gys); Serial.print("\t");
    Serial.println(gzs);
    Serial.println("\n-----------------");
}

