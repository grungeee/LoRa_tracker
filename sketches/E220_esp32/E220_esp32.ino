#include "Arduino.h"
#include "LoRa_E220.h"
#include <HardwareSerial.h>

#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 21 //  TX2  in example
#define RX_PIN 20 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 3 // 21 in example
/*

#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example
*/

#define comChan 69 

#define count 0
// Define e220ttl object only if it's not defined
LoRa_E220 e220ttl(RX_PIN, TX_PIN, &Serial1, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);

void setup()
{
  // Start the serial communication with the GPS module
// Start the serial communication with the computer
  Serial.begin(115200);



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


/*
void loop()
{
  Serial.println("Hi, I'm going to send message!");

  // Send message
  ResponseStatus rs = e220ttl.sendBroadcastFixedMessage(69, String(count));
  count =+
}
*/

void loop() {
  // Set an LED to blue
  LEDR_COLOR(8, blue, 500);

  // Send a message
  Serial.println("Sending...[Greet4]");
  e220ttl.sendMessage("Hello, my name is UNO4! What's your name?");
  delay(500);

  // Receive message
  ResponseContainer rc = e220ttl.receiveMessage();
  if (rc.status.code == 1) {
    Serial.println("Receiving...");
    Serial.println(rc.data);

    Serial.println("Answering...");
    e220ttl.sendMessage("Hi, my name is UNO4");

    if (rc.data == "") {
      Serial.println("Can't hear anything...");
    }

    LEDR_COLOR(8, green, 500);
  } else {
    Serial.println("So lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
      LEDR_COLOR(i, green, 100);
    }
  }
}


// --------- E220 reieved data ---------
  ResponseContainer rc = e220ttl.receiveMessage();
// --------- E220 reieved data ---------
  if (rc.status.code == 1) {
    Serial.println("Receiving...");
    Serial.println(rc.data);

    Serial.println("Answerting...");
    e220ttl.sendMessage("Hi, my name is UNO4");
    if (rc.data == "")
      {
      Serial.println("cant hear anything...");
      }

// --------------- LED RING ----------------
 // LEDR_COLOR(8,green, 500);
//pixels.clear();
//pixels.show();
//pixels.setPixelColor(8, pixels.Color(0, 255, 0));
//pixels.show();

//delay(500);
//pixels.clear();
//pixels.show();
// -----------------------------------------

 }
  else {
    Serial.println("so lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
// --------------- LED RING ----------------
      LEDR_COLOR(i,green, 100);
    }
// -----------------------------------------
  }
}

