# LoRa_tracker

## Hardware requirements

- Arduino Uno or ESP32 development board (examples use the **ESP32 Dev Module**)
- Ebyte E220 LoRa module
- NEO GPS module with antenna
- BNO055 or GY-9250 IMU
- 1.28" LCD display
- 16-bit WS2812 LED ring
- Power supply (LiPo battery, step-up and step-down modules)
- Breadboard and wiring

## Building the main sketch

The `GY-BNO055_LED_RING` example demonstrates the core functionality of
reading orientation data and driving the LED ring. It can be compiled
with either the Arduino IDE or PlatformIO.

### Arduino IDE
1. Install the Espressif **ESP32** boards through the Board Manager and
   select `ESP32 Dev Module`.
2. Copy the folders from `libraries/` into your Arduino libraries directory.
3. Open `sketches/GY-BNO055_LED_RING/GY-BNO055_LED_RING.ino` and press
   **Upload**.

### PlatformIO
1. Create a new project with the `esp32dev` board.
2. Add the libraries listed below to the `lib_deps` section of
   `platformio.ini`.
3. Place the contents of `sketches/GY-BNO055_LED_RING` into the `src`
   directory and run `platformio run`.

### Dependencies

- `EByte_LoRa_E220_library`
- `TinyGPSPlus`
- `Adafruit_BNO055` and `Adafruit_Unified_Sensor`
- `Adafruit_NeoPixel`
- Display libraries such as `Adafruit_GC9A01A` if using the LCD

A copy of these libraries is included in the `libraries/` folder.

### Project status

- [x] LoRa Module works on Arduino and ESP32
- [x] GPS Module verified on both platforms
- [x] 16-bit LED ring operational
- [ ] 1.28" LCD used for troubleshooting only
- [ ] GY-9250 compass needs more work
- [ ] Power system under development
- [ ] Enclosure still experimental

## License

This project is licensed under the terms of the [MIT License](LICENSE).

## Example GPS LoRa sketches

Two example sketches demonstrate sending GPS data over an E220 LoRa radio.
`GPSv3_esp32_LoRa_TX` reads the NEO GPS on an ESP32 and sends latitude,
longitude, altitude and satellite count as a binary structure.
`GPSv3_esp32_LoRa_RX` receives the structure and prints the values to the
serial monitor.
