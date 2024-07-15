/*
 * LoRa E220
 * Write on serial to transfer a message to other device
 * by Renzo Mischianti <https://mischianti.org>
 * https://mischianti.org/category/my-libraries/lora-e220-llcc68-devices/
 *
 * E220        ----- Arduino UNO
 * M0         ----- GND
 * M1         ----- GND
 * TX         ----- PIN 2 (PullUP)
 * RX         ----- PIN 3 (PullUP & Voltage divider)
 * AUX        ----- Not connected
 * VCC        ----- 3.3v/5v
 * GND        ----- GND
 *
 */
#include "Arduino.h"
#include "LoRa_E220.h"
 
LoRa_E220 e220ttl(2, 3); // Arduino RX --> e220 TX - Arduino TX --> e220 RX
 
void setup() {
  Serial.begin(9600);
  delay(500);
 
  Serial.println("Hi, I'm going to send message!");
 
  // Startup all pins and UART
  e220ttl.begin();
 
  // Send message
  ResponseStatus rs = e220ttl.sendMessage("Hello, world?");
  // Check If there is some problem of successfully send
  Serial.println(rs.getResponseDescription()); // -> Success
}
 
void loop() {
    // If something available
  if (e220ttl.available()>1) {
      // read the String message
    ResponseContainer rc = e220ttl.receiveMessage();
    // Is something goes wrong print error
    if (rc.status.code!=1){
        rc.status.getResponseDescription();
    }else{
        // Print the data received
        Serial.println(rc.data);
    }
  }
  if (Serial.available()) {
      String input = Serial.readString();
      e220ttl.sendMessage(input);
  }
}
