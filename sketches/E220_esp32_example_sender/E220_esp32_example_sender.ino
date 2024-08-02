// With FIXED SENDER configuration
//#define DESTINATION_ADDL 3
//#define FREQUENCY_868

// With FIXED RECEIVER configuration
//#define DESTINATION_ADDL 2

// If you want use RSSI uncomment //#define ENABLE_RSSI true
// and use relative configuration with RSSI enabled
//#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h"

#define DESTINATION_ADDL BROADCAST_ADDRESS
#define FREQUENCY_868
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

// ---------- esp32 pins --------------
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too

void setup() {
  Serial.begin(115200);
  delay(500);

  // Startup all pins and UART
  e220ttl.begin();

ResponseStructContainer c;
c = e220ttl.getConfiguration();
Configuration configuration = *(Configuration*) c.data;
 
configuration.ADDL = BROADCAST_ADDRESS;
configuration.ADDH = BROADCAST_ADDRESS;
//	configuration.ADDL = 0x03;  // First part of address
//	configuration.ADDH = 0x03; // Second part
 
configuration.CHAN = 22;
 
configuration.SPED.uartBaudRate = UART_BPS_9600;
configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
configuration.SPED.uartParity = MODE_00_8N1;
 
configuration.OPTION.subPacketSetting = SPS_200_00;
configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
configuration.OPTION.transmissionPower = POWER_22;
 
configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION; // FT_FIXED_TRANSMISSION; //repeater mode off
configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
         
e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
c.close();
 

  Serial.println("Hi, I'm going to send message!");

  // Send message
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
  // Check If there is some problem of succesfully send
  Serial.println(rs.getResponseDescription());
}

void loop() {
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(23, "Hello, world?");
  rs = e220ttl.sendBroadcastFixedMessage(24, "hiiiiiiiiiiiiii");
  rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 23, "Hello @ 23");
  rs = e220ttl.sendFixedMessage(0, DESTINATION_ADDL, 24, "Hello, @ 24");
  rs = e220ttl.sendMessage("Someone?!");

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
	  // Check If there is some problem of succesfully send
	  Serial.println(rs.getResponseDescription());
  }
}
