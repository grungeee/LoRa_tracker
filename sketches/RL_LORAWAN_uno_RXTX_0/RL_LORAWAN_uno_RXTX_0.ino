
// =========================< REFS >===============================
// ----------------------------------------------------------------
// links:
// https://chatgpt.com/g/g-GbLbctpPz-universal-primer/c/6e47c3d8-e0eb-4122-8c0d-e780e3d562fe

// ----------------------------------------------------------------
// Connections:
// E220 Module -> Arduino
// M0 -> GND
// M1 -> GND
// RXD -> TX (Pin 1)
// TXD -> RX (Pin 0)
// AUX -> (Optional for status monitoring, e.g., Pin 4)
// VCC -> 3.3V
// GND -> GND
// NSS -> Pin 10
// DIO0 -> Pin 2
// RST -> Pin 9
// BUSY -> Pin 3
// ----------------------------------------------------------------


#include <RadioLib.h>

// Create a new instance for the E220 module
SX1262 radio = new Module(10, 2, 9, 3); // NSS, DIO0, RST, BUSY

void setup() {
  // Initialize Serial Monitor
  Serial.begin(9600);

  // Initialize radio
  int state = radio.begin();
  if (state == ERR_NONE) {
    Serial.println("Radio initialized successfully!");
  } else {
    Serial.print("Failed to initialize radio, code: ");
    Serial.println(state);
    while (true);
  }

  // Set radio parameters
  radio.setFrequency(915.0); // Frequency in MHz, set according to your region
  radio.setBandwidth(125.0);
  radio.setSpreadingFactor(7);
  radio.setCodingRate(5);
  radio.setOutputPower(14);
}

void loop() {
  // Example: Sending a message
  String message = "Hello, world!";
  int state = radio.transmit(message);
  if (state == ERR_NONE) {
    Serial.println("Message sent successfully!");
  } else {
    Serial.print("Failed to send message, code: ");
    Serial.println(state);
  }

  // Wait for a second before sending the next message
  delay(1000);
}


