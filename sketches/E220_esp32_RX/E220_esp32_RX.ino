
// /==========================================/ START /===================================/
#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>
#include "math.h"

// |==========================< LED RING INCLUDE >===========================|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21 // NOT GOOD FOR SPI
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
/*
    void loopXTimes (int count){
    int loopIterValue = 0;
        for (int i = 0; i < count; i++) {
            loopIterValue = i;
        }
    reutrn loopIterValue;
    }
    */
// |================================================|


#define ENABLE_RSSI true

// ---------- esp32 pins --------------
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

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


  Serial.begin(115200);
    while (!Serial) {}; // wait until serial port is opened

  
  e220ttl.begin(); // Startup all pins and UART

    // ------------------------ config ----------------------
//	ResponseStructContainer c;
//	c = e220ttl.getConfiguration();
	//Configuration configuration = *(Configuration*) c.data;
    // configuration.CHAN = 23; // Communication channel
//	configuration.ADDL = 0x03;  // First part of address
//	configuration.ADDH = 0x00; // Second part
    // ----------------------------------------------

    
    //Serial.print(String(loopXTimes(5)));
    //LEDR_COLOR(loopXTimes(16), yellow, 100);
    
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDR_COLOR(0,pink,1000);
    delay(1000);
    LEDcycle(yellow);
    Serial.println("YELLOW: Start receiving!");

}


// =============================<< LOOP >======================================
void loop() {
    LEDR_COLOR(0,dim_white,1000);
    delay(1000);

    // ------------------ GPS calc --------------------
  // Example coordinates (latitude, longitude) in degrees
    /*
  double lat1 = 37.7749;
  double lon1 = -122.4194;
  double lat2 = 34.0522;
  double lon2 = -118.2437;

  double distance = calculateDistance(lat1, lon1, lat2, lon2);
  double bearing = calculateBearing(lat1, lon1, lat2, lon2);

  Serial.print("---------------------------");
  Serial.print("Distance (m): ");
  Serial.println(distance);

  Serial.print("Bearing (radians): ");
  Serial.println(bearing);
  Serial.print("---------------------------");
  */
    // --------------------------------------------------
	// If something available
  if (e220ttl.available()>1) {
	  Serial.println("GREEN:Message received!");

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
    LEDR_COLOR(8,chill, 1000);
    LEDR_COLOR(8,pink, 1000);
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
