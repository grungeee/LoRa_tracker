#include <Arduino.h>
#include <LoRa_E220.h>
#include <HardwareSerial.h>

// |=============< LED RING INCLUDE >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21
#define LEDS_NUM 16

int red[3] = {255, 0, 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);

void LEDR_COLOR(int ledIndex, int color[3], int delayTime) {
  pixels.clear();
  pixels.setPixelColor(ledIndex, pixels.Color(color[0], color[1], color[2]));
  pixels.show();
  delay(delayTime);
}
// |=============< LED RING INCLUDE >==============|

HardwareSerial MySerial(2);
// |===================< E220 >==================|
#define TX_PIN 17
#define RX_PIN 16
#define AUX_PIN 4
#define M0_PIN 2
#define M1_PIN 15

LoRa_E220 e220ttl(TX_PIN, RX_PIN, &MySerial, AUX_PIN, UART_BPS_RATE_9600, SERIAL_8N1);

void configureChannel() {
    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

    configuration.CHAN = 23; // Assign the desired channel (0-31)

    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
}
// |=============================================|

void setup() {
  // Initialize Serial
  Serial.begin(9600);
  while (!Serial);

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
  // |=============================================|
}

void loop() {
  LEDR_COLOR(8, blue, 500);

  Serial.println("Sending...[Greet4]");
  e220ttl.sendMessage("Hello, my name is UNO4! What's your name?");
  delay(500);

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
