
#include <SPI.h>
#include <LoRa.h>

// Define the channels you want to use
const long channels[] = {915E6, 916E6, 917E6}; // Example frequencies in Hz
int currentChannel = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Channel Setup");

  // Initialize LoRa module
  if (!LoRa.begin(channels[currentChannel])) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.print("Current Channel: ");
  Serial.println(channels[currentChannel]);
}

void loop() {
  // Example of sending a message
  Serial.print("Sending packet: ");
  Serial.println("hello");

  // Send packet
  LoRa.beginPacket();
  LoRa.print("hello");
  LoRa.endPacket();

  delay(1000);

  // Switch to the next channel
  currentChannel = (currentChannel + 1) % (sizeof(channels) / sizeof(channels[0]));
  LoRa.setFrequency(channels[currentChannel]);

  Serial.print("Switched to Channel: ");
  Serial.println(channels[currentChannel]);

  delay(5000); // Wait for 5 seconds before sending the next packet
}
