# Build Process and Wiring

This document describes how to assemble the LoRa tracker prototype and connect all modules to the ESP32.

## Required Parts

- ESP32-WROOM-32D development board
- E220-900T30D LoRa transceiver
- NEO-6 GPS module
- GY-9250 IMU
- 1.28" GC9A01 round LCD
- WS2812 16-bit LED ring
- LiPo battery with USB‑C charger board
- Step-up and step-down voltage regulators
- Breadboard and jumper wires

## Assembly Steps

1. **Prepare the boards** – Solder header pins to the ESP32 and any breakout modules that require them.
2. **Mount components** – Place the ESP32, LoRa module, GPS module and IMU on the breadboard. Keep the LCD and LED ring off-board if needed.
3. **Wire the LoRa module** – Connect the E220 module pins to the ESP32 as shown in the diagram below. Make sure `M0` and `M1` are either left floating or pulled low for normal mode.
4. **Wire the GPS module** – Connect GPS TX to ESP32 pin 33 and GPS RX to ESP32 pin 32. Provide 5 V and GND from the power rail.
5. **Wire the LCD** – Use the SPI pins from the ESP32 (`SDA` 23 and `SCL` 18) along with the chip‑select pins (`CS` 5, `DC` 4, `RST` 19) as listed in the pinout table.
6. **Wire the LED ring** – Connect the data input of the LED ring to ESP32 pin 21 and power it from the 5 V rail.
7. **Wire the IMU** – Connect the `SDA` and `SCL` lines of the GY‑9250 to ESP32 pins 21 and 22. The sensor uses the standard I²C voltage of 3.3 V.
8. **Power distribution** – Use the USB‑C charger board to supply the LiPo battery and step-down regulator for the ESP32. The step-up regulator can provide a stable 5 V rail for the peripherals.
9. **Double-check connections** – Verify the wiring matches the schematic before powering the board. Incorrect wiring can damage the modules.

## Wiring Diagram

The following Mermaid diagram shows the logical wiring. Pin numbers correspond to the example pinout in the README.

```mermaid
flowchart TB

    %% === POWER SECTION ===
    USB[🧲 USB Input (5V)] --> TP4056[🔌 TP4056 Charger Module]
    TP4056 -->|B+ / B-| LiPo[🔋 LiPo Battery (3.7V)]
    LiPo --> Boost[⚡ Step-Up Converter (to 5V)]
    Boost --> V5[🔋 5V Rail]

    %% === POWER DISTRIBUTION ===
    V5 -->|VIN Pin| ESP32[🧠 ESP32 Dev Board]
    ESP32 -->|3.3V Regulator| V3[🔌 3.3V Devices Rail]

    %% === DEVICES USING 3.3V ===
    V3 --> E220[📡 E220 LoRa Module]
    V3 --> GPS[📍 GPS Module]
    V3 --> IMU[🧭 9-Axis IMU Sensor]

    %% === DEVICES USING 5V ===
    V5 --> LEDring[💡 LED Ring (Neopixels?)]
    V5 --> LCD[🖥️ LCD Display]

    %% === SIGNAL CONNECTIONS ===
    ESP32 -->|TX (17)| E220
    ESP32 -->|RX (16)| E220
    ESP32 -->|M0 (2)| E220
    ESP32 -->|M1 (15)| E220
    ESP32 -->|AUX (4)| E220

    ESP32 -->|RX (33)| GPS
    ESP32 -->|TX (32)| GPS

    ESP32 -->|SDA (21)| IMU
    ESP32 -->|SCL (22)| IMU

    ESP32 -->|GPIOs| LEDring
    ESP32 -->|GPIO 23| LCD
    ESP32 -->|GPIO 18| LCD
    ESP32 -->|GPIO 5| LCD
    ESP32 -->|GPIO 4| LCD
    ESP32 -->|GPIO 19| LCD
```

This diagram is intended as a reference; you can adapt the exact pin numbers as needed. Ensure that all 5 V peripherals share the common ground and that the ESP32 is supplied with 3.3 V from the step‑down regulator.

