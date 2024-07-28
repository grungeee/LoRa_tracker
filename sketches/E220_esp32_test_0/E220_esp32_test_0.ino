// |==============================< LIBRARIES >===============================|
#include <Arduino.h>

// |=============< LED RING PRESETS >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21
#define LEDS_NUM 16 // number of LEDs on the ring duh

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

int red[3] = {255, 0, 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};
int yellow[3] = {255, 255, 0};
int dim_yellow[3] = {20, 20, 0};
int dim_white[3] = {20, 20, 20};

//led_bit, color, wait
void LEDR_COLOR(int ledIndex, int color[3], int delayTime) {
  pixels.clear();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
}

// |======================< E220 PRESETS >======================|
#include <LoRa_E220.h>
#include <HardwareSerial.h>

// Remove this line as Serial2 is already defined in the ESP32 core
// HardwareSerial Serial2(2);

// Configs
#define FREQUENCY_868

// Pins
#define TX_PIN 17 //  TX2  in example
#define RX_PIN 16 //  RX2  in example
#define AUX_PIN 4 // 18 in exaple
    // Just drop the mohterfucking M0 and M1 pins
    // --> GND this bitch
#define M0_PIN 2 // 19 in example
#define M1_PIN 15 // 21 in example

//LoRa_E220 e220ttl(&Serial2, 15, 21, 19);


// LoRa_E220 e220ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e220 TX, esp32 TX --> e220 RX AUX M0 M1
LoRa_E220 e220ttl(&Serial2, 17, 16, 4, 21, 15, UART_BPS_RATE_9600)); // will it work?
LoRa_E220 e220ttl(&Serial2, 17, 16, 4, 21, 15, UART_BPS_RATE_9600)); // will it work?


// Configuration
void configureChannel() {
    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

    configuration.CHAN = 23; // Assign the desired channel (0-31)

    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
}

//////////////////////////////////////////////////////////////////////////////////////////////

// |==============================< SETUP >===============================|
void setup() {
  // ----------- [ Standard Setup ] -----------
  Serial.begin(9600); // Initialize Serial || Alternative BDR: 115200
  while (!Serial); //This line waits for the serial port to be ready before continuing with the rest of the program.

  // |=============<  E220 SETUP >==============|
  Serial2.begin(9600); // TX = GPIO17, RX = GPIO16

  // Initialize E220 module
  e220ttl.begin();

  // Configure E220 initial settings
  Configuration configuration;
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;  // Example data rate, adjust accordingly
  configuration.SPED.uartBaudRate = UART_BPS_RATE_9600;
  configuration.SPED.uartParity = MODE_00_8N1;
  configuration.OPTION.transmissionPower = POWER_22;
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_ENABLED;
  
  e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  Serial.println("E220 module initialized.");

  // Configure Channel
  configureChannel();

  // |=============< LED RING SETUP >==============|
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
  #endif

  pixels.begin(); 
  pixels.clear();
  pixels.show();

  // ------ init led ring animation  test ------
  for (int i = 0; i < LEDS_NUM; i++) {
    LEDR_COLOR(i, dim_yellow, 100);
  }

  // |=============================================|
}


// |====================================< LOOP >=========================================|
void loop() {
  Serial.println("Sending...[Greet4]");
  e220ttl.sendMessage("Hello, my name is UNO4! What's your name?");
  LEDR_COLOR(8, blue, 500);

  ResponseContainer rc = e220ttl.receiveMessage();
  if (rc.status.code == 1) {
    Serial.println("Receiving...");
    Serial.println(rc.data);
    LEDR_COLOR(8, green, 500);

    if (rc.data == "") {
      Serial.println("Can't hear anything...");
      LEDR_COLOR(8, red, 500);
    }

    Serial.println("Answering...");
    e220ttl.sendMessage("Hi, my name is UNO4");
    LEDR_COLOR(8, yellow, 500);
    LEDR_COLOR(9, yellow, 500);
    LEDR_COLOR(10, yellow, 500);

  } else {
    Serial.println("So lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
      LEDR_COLOR(i, dim_white, 100);
    }
  }
}
