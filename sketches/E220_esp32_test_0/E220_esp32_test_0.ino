// |==============================< LIBRARIES >===============================|
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <Arduino.h> // What do I need this for again?

// |=============< LED RING PRESETS >==============|
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define LED_RING_PIN 21
#define LEDS_NUM 16 // number of LEDs on the ring duh

Adafruit_NeoPixel pixels(LEDS_NUM, LED_RING_PIN, NEO_GRB + NEO_KHZ800);


// --------[ Fancy LED Colors Function ]--------
int red[3] = {255, 0, 0};
int green[3] = {0, 255, 0};
int blue[3] = {0, 0, 255};
int yellow[3] = {0, 255, 255};
int dim_yellow[3] = {0, 20, 20};
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

HardwareSerial Serial2(2); // Create an instance of HardwareSerial for UART2

#define TX_PIN 17
#define RX_PIN 16
#define AUX_PIN 4
#define M0_PIN 2
#define M1_PIN 15

//LoRa_E220 e220ttl(TX_PIN, RX_PIN, &Serial2, AUX_PIN, UART_BPS_RATE_9600, SERIAL_8N1);
LoRa_E220 e220ttl(&Serial2, 15, 21, 19);

void configureChannel() {
    ResponseStructContainer c;
    c = e220ttl.getConfiguration();
    Configuration configuration = *(Configuration*) c.data;

    configuration.CHAN = 23; // Assign the desired channel (0-31)

    e220ttl.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
}

//////////////////////////////////////////////////////////////////////////////////////////////



// |==============================< SETUP >===============================|
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void setup() {
  // ----------- [ Standard Setup ] -----------
  Serial.begin(115200); // Initialize Serial
  while (!Serial); //This line waits for the serial port to be ready before continuing with the rest of the program.


  // |=============<  E220 SETUP >==============|
 // Initialize Serial2 with specific TX and RX pins
  Serial2.begin(9600, SERIAL_8N1, 17, 16); // TX = GPIO17, RX = GPIO16

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

    Serial.println("Answering...");
    e220ttl.sendMessage("Hi, my name is UNO4");
    LEDR_COLOR(8, green, 500);
    LEDR_COLOR(9, green, 500);
    LEDR_COLOR(10, green, 500);

    if (rc.data == "") {
      Serial.println("Can't hear anything...");
      LEDR_COLOR(8, red, 500);
    }
  } else {
    Serial.println("So lonely :(");
    for (int i = 0; i < LEDS_NUM; i++) {
      LEDR_COLOR(i, dim_white, 100);
    }
  }
}
