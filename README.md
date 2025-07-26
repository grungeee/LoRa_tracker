# LoRa_tracker
## Description
A LoRa-based tracker that transmits GPS data using an ESP32 and E220 radio module. The project includes an LCD and IMU for orientation feedback.

## Tasks and Milestones

### Hardware
- [x] Breadboard prototype with LoRa and GPS modules
- [x] Verified LoRa communication on Arduino and ESP32
- [x] LED ring proof of concept
- [x] Preliminary power setup using USB‑C charging board
- [ ] Finalize 1.28" LCD user interface and menu navigation
- [ ] Integrate and calibrate GY-9250 magnetometer for compass heading
- [ ] Implement power management with charging and voltage regulation
- [ ] Design and print a durable enclosure for field use

### Software
- [x] Basic LoRa send/receive sketches
- [x] GPS data parsing
- [ ] Improve LoRa error handling and data validation
- [ ] Display distance and bearing on the LCD
- [ ] Provide configuration menu and serial debugging output

### Validation
- [ ] Assemble a complete prototype with all modules
- [ ] Battery endurance tests
- [ ] Perform field tests to verify range, accuracy and battery life

### Documentation & Release
- [x] Document quick setup and example sketches
- [x] Document the build process and wiring diagrams
- [ ] Prepare the release package and source files
- [ ] Create a printable PDF manual

## Resources

- [ESP32-WROOM-32D datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32_datasheet_en.pdf)
- [E220-900T30D User Manual](E220-900T30D_UserManual_EN_v1.0.pdf)
- [NEO-6 GPS module datasheet](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf)
- [MPU-9250 datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-9250-Datasheet.pdf)
- [GC9A01 LCD datasheet](https://cdn-shop.adafruit.com/product-files/5220/GC9A01.pdf)
- [WS2812 LED ring datasheet](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf)

### ESP32-WROOM-32D Pinout

![ESP32-WROOM-32D pin layout](https://docs.espressif.com/projects/esp-idf/en/latest/_static/esp32-wroom-32d-pin-layout.png)

The ESP32-WROOM-32D module exposes 38 pins. Refer to the datasheet for the full description of each pin.

### Materials

- [Arduino Uno](https://docs.arduino.cc/hardware/uno-rev3)
- Breadboard

- [E220-900T30D LoRa module](E220-900T30D_UserManual_EN_v1.0.pdf)
- [NEO-6 GPS module](https://www.u-blox.com/sites/default/files/products/documents/NEO-6_DataSheet_%28GPS.G6-HW-09005%29.pdf) (with GPS antenna)
- [GY-9250 IMU](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-9250-Datasheet.pdf)
- [1.28" GC9A01 LCD](https://cdn-shop.adafruit.com/product-files/5220/GC9A01.pdf)
- [WS2812 16-bit LED ring](https://cdn-shop.adafruit.com/datasheets/WS2812.pdf)

- Power
    - Lipo
    - StepUp
    - StepDown

### Progress Functionality


- [x] LoRa Module
    - works on Arduino
    - works on esp32

- [x] GPS Module
    - worked like a charm both on arduino and esp32

- [x] 16Bit LEd-Ring
    - it works, the actual code is dependant on the GPS and GY        

- [ ] 1.28" LCD
    - could use it for troubleshooting atm, actual UX/UI is the last thing that needs to be done

- [ ] GY-9250
    - got some work done on GY-512
    - need to redo for compass
    - magnetometer still not functional with available units

- [ ] Power
    - used a tiny USB-C charging board (USB/IN,BAT,OUT[5V])
    - need 18650 with lower current, 40A cell is not charging
    - StepDown for ESP32!

- [ ] Enclosure
    - The first prototype was messy but it holds the protoboard and components.


    

## Example GPS LoRa sketches

Two example sketches demonstrate sending GPS data over an E220 LoRa radio. `GPSv3_esp32_LoRa_TX` reads the NEO GPS on an ESP32 and sends latitude, longitude, altitude and satellite count as a binary structure. `GPSv3_esp32_LoRa_RX` receives the structure and prints the values to the serial monitor.
`E220_GOOD_PRACTICE_RX` extends the receiver by integrating the LED ring and round LCD used in the good practice example to display the incoming coordinates. The sketch now calculates the distance and bearing from a fixed receiver position  to the received location and shows these values on screen.

## Compiling and Flashing the Sketches

The example sketches can be built with either the Arduino IDE or PlatformIO. The repository already contains the necessary library sources under the `libraries` folder, but you can also install them through the Arduino Library Manager. To compile the sketches:

1. **Install board definitions**
   - ESP32 boards by Espressif (`esp32` package) for the ESP32 based examples.
   - Arduino AVR Boards for the classic Arduino Uno sketches.
2. **Install required libraries**
   - **EByte LoRa E220 library** (LoRa_E220)
   - **TinyGPSPlus** for parsing GPS messages
   - **Adafruit GC9A01A** display driver (requires *Adafruit GFX* and *Adafruit BusIO*)
   - **Adafruit NeoPixel** for the LED ring
3. Open the desired sketch from the `sketches` directory (for example `GPSv3_esp32_LoRa_TX` or `E220_GOOD_PRACTICE_RX`).
4. Select the correct board and port, then compile and upload.

Using PlatformIO, create a new project for the appropriate board and copy the sketch contents into `src/main.cpp`, ensuring the libraries listed above are added to `platformio.ini`.


## Sensor and Module Pinout

| Component | Example ESP32 Pins |
|-----------|-------------------|
| **E220 LoRa module** | `TX_PIN 17`, `RX_PIN 16`, `AUX_PIN 4`, `M0_PIN 2`, `M1_PIN 15` |
| **NEO-6 GPS module** | `RXPinGPS 33` (GPS→ESP32), `TXPinGPS 32` (ESP32→GPS) |
| **GC9A01A LCD** | `TFT_CS 5`, `TFT_RST 19`, `TFT_DC 4`, `TFT_SDA 23`, `TFT_SCL 18` |
| **WS2812 LED ring** | `LED_RING_PIN 21` |
| **MPU9250 IMU** | `SDA_PIN 21`, `SCL_PIN 22` |
| **BNO055 IMU** | Default I²C pins `SDA 21` and `SCL 22` |


## Build Documentation

Step-by-step build instructions and a wiring diagram are available in
[docs/BUILD_AND_WIRING.md](docs/BUILD_AND_WIRING.md).


