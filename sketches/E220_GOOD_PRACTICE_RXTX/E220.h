// ========================<< Ebyte E220 LoRa >>=====================================
#ifndef E220_H
#define E220_H

#include "Arduino.h"
#include "LoRa_E220.h"

// ---------- esp32 pins --------------
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define FREQUENCY_868
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

#define comChan 69

extern LoRa_E220 e220ttl;

void initE220();
void configE220();
void sendE220Message(int channel, const String& message);
void receiveE220Message();

#endif
