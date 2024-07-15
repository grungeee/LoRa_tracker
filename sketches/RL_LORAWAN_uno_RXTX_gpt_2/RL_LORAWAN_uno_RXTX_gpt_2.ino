// https://chatgpt.com/g/g-NgAcklHd8-scispace/c/e2867780-2964-4143-8700-5d4eb946a7e9


#include <RadioLib.h>

// Create a new instance for your E220 LLCC68 module
SX1268 radio = new Module(10, 9, 2, 3); // Pins may vary based on your setup

void setup() {
  Serial.begin(9600);
  
  // Initialize the module with the default settings for SX126x LoRa modem
  int state = radio.begin(434.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8, 1.6, false);

  if (state == RADIOLIB_ERR_NONE) {
    Serial.println("Initialization successful!");
  } else {
    Serial.print("Initialization failed, code ");
    Serial.println(state);
    while (true);
  }

  // Additional configuration (as per the default configuration)
  radio.setCurrentLimit(60.0);
  radio.setDio2AsRfSwitch(true);
  radio.explicitHeader();
  radio.setCRC(2);
}

void loop() {
  // Your main code to send/receive data
}
