/*
  LoRa Demo 3
  lora-demo3.ino
  Bi-directional LED control (duplex communications)
  Requires LoRa Library by Sandeep Mistry - https://github.com/sandeepmistry/arduino-LoRa
  sendMessage & onReceive functions based upon "LoRaDuplexCallback" code sample by Tom Igoe

  DroneBot Workshop 2023
  https://dronebotworkshop.com
*/

// Include required libraries
#include <SPI.h>
#include <LoRa.h>

// |=============< LED RING >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// |=======================================|


#define LED_RING_PIN 10
#define LEDS_COUNT 16
// LED connection
//const int LED_RING_PIN = 10;

Adafruit_NeoPixel pixels(LEDS_COUNT, LED_RING_PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500


// Define the pins used by the LoRa module
const int csPin = 4;     // LoRa radio chip select
const int resetPin = 2;  // LoRa radio reset
const int irqPin = 3;    // Must be a hardware interrupt pin


// Outgoing message variable
String outMessage;

// Message counter
byte msgCount = 0;

// Receive message variables
String contents = "";
String buttonPress = "button pressed";
bool rcvButtonState;

// Source and destination addresses
byte localAddress = 0xBB;  // address of this device
byte destination = 0xFF;   // destination to send to

// Pushbutton variables
int buttonPin = 7;
int sendButtonState;
int LEDnum = 0;

void setup() {


  // Set pushbutton as input
  //pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin,INPUT);

  // Set LED as output
  //pinMode(ledPin, OUTPUT);
// |=============< LED RING >==============|
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

// |=======================================|

  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa Duplex with callback");

  // Setup LoRa module
  LoRa.setPins(csPin, resetPin, irqPin);

  // Start LoRa module at local frequency
  // 433E6 for Asia
  // 866E6 for Europe
  // 915E6 for North America

  if (!LoRa.begin(866E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // Set Receive Call-back function
  LoRa.onReceive(onReceive);

  // Place LoRa in Receive Mode
  LoRa.receive();

  Serial.println("LoRa init succeeded.");
}

void loop() {




  // Get pushbutton state
  sendButtonState = digitalRead(buttonPin);

  // Send packet if button pressed
  if (sendButtonState == HIGH) {

    // Compose and send message
    outMessage = buttonPress;
    sendMessage(outMessage);
    delay(500);

    // Place LoRa back into Receive Mode
    LoRa.receive();
  }
}

// Send LoRa Packet
void sendMessage(String outgoing) {
  LoRa.beginPacket();             // start packet
  LoRa.write(destination);        // add destination address
  LoRa.write(localAddress);       // add sender address
  LoRa.write(msgCount);           // add message ID
  LoRa.write(outgoing.length());  // add payload length
  LoRa.print(outgoing);           // add payload
  LoRa.endPacket();               // finish packet and send it
  msgCount++;                     // increment message ID
}

// Receive Callback Function
void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return

  // Read packet header bytes:
  int recipient = LoRa.read();        // recipient address
  byte sender = LoRa.read();          // sender address
  byte incomingMsgId = LoRa.read();   // incoming msg ID
  byte incomingLength = LoRa.read();  // incoming msg length

  String incoming = "";  // payload of packet

  while (LoRa.available()) {        // can't use readString() in callback, so
    incoming += (char)LoRa.read();  // add bytes one by one
  }

  if (incomingLength != incoming.length()) {  // check length for error
    Serial.println("error: message length does not match length");
    return;  // skip rest of function
  }

  // If the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;  // skip rest of function
  }

  // If message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();


  // Toggle button state
  if (incoming.equals(buttonPress)) {
    rcvButtonState = !rcvButtonState;
  }

  // Drive LED
  if (rcvButtonState == true) {
// --------------- LED RING ----------------
  LEDnum = msgCount;
  pixels.clear();
  pixels.setPixelColor(LEDnum, pixels.Color(0, 255, 0));
  pixels.show();
// -----------------------------------------
    //digitalWrite(ledPin, HIGH);
    Serial.println("led on");
  } else {
// --------------- LED RING 2 --------------
  pixels.clear();
  pixels.setPixelColor(LEDnum, pixels.Color(0, 0, 255));
  pixels.show();
// -----------------------------------------
    //digitalWrite(ledPin, LOW);
    Serial.println("led off");
  }
}
