/*
  Simple RadioLib example.

  This example shows basic usage of the library.
  It will automatically detect the pinout of your
  wireless board as well as the radio type.

  This example assumes RadioLib is being used
  as the radio driver:
  https://github.com/jgromes/RadioLib
*/

// include RadioLib
#include <RadioLib.h>

// automatically detect which board is being used
#define RADIO_BOARD_AUTO

// now include RadioBoards
// this must be included AFTER RadioLib!
#include <RadioBoards.h>

// create a new RadioLib module
// "Radio" will be set to the correct module type,
// and "RadioModule" will have the correct pins
Radio radio = new RadioModule();

void setup() {
  Serial.begin(9600);

  // initialize the radio
  Serial.print(F("[Radio] Initializing ... "));
  int state = radio.begin();
  if (state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  // whatever else here - transmitting, receiving etc.
}

