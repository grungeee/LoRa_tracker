/*
 * EBYTE LoRa E220
 *
 * Receive messages on CHANNEL 23
 * Uncomment #define ENABLE_RSSI true in this sketch
 * if the sender send RSSI also
// If you want use RSSI uncomment
#define ENABLE_RSSI true
 */

#include "Arduino.h"
#include "LoRa_E220.h"


//#define ENABLE_RSSI true

// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

void setup() {
  Serial.begin(9600);
    while (!Serial) {}; // wait until serial port is opened

  // Startup all pins and UART
  e220ttl.begin();

  Serial.println("YELLOW: Start receiving!");
}

void loop() {
	// If something available
  if (e220ttl.available()>1) {
	  Serial.println("GREEN:Message received!");

	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220ttl.receiveMessageRSSI();
#else
	ResponseContainer rc = e220ttl.receiveMessage();
#endif // Is something goes wrong print error if (rc.status.code!=1){ Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
}

///////////////////////////////

/*
 * EBYTE LoRa E220
 *
 * Receive messages on CHANNEL 23
 * Uncomment #define ENABLE_RSSI true in this sketch
 * if the sender send RSSI also
// If you want use RSSI uncomment
#define ENABLE_RSSI true

#include "Arduino.h"
#include "LoRa_E220.h"


// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600); // -> this does too
// -------------------------------------

void setup() {
  Serial.begin(9600);
    while (!Serial) {}; // wait until serial port is opened

  // Startup all pins and UART
  e220ttl.begin();

  Serial.println("Start receiving!");
}

void loop() {
	// If something available
  if (e220ttl.available()>1) {
	  Serial.println("Message received!");

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
}
*/
