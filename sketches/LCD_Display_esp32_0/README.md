### LCD Display for esp32

now i can print 4 Variables on the display



### pinout
```c
// Define the pins used for the display
#define TFT_CS    5   // Chip select control pin
#define TFT_RST   19
#define TFT_DC    4
#define TFT_SDA   23  // SPI Data (MOSI)
#define TFT_SCL   18  // SPI Clock (SCK)
// old works too but used for LORA
//#define TFT_RST   22  // Reset pin
//#define TFT_DC    21  // Data Command control pin

```

### Problem: Text is note centered and is being cut off
> How to center the text
```c
  // Center text vertically and horizontally
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(2);
  tft.setTextColor(GC9A01A_WHITE);
```


### Problem The display is flipped relative to the printed pinout text (on the pcb)
> How to flip the display 180 deg h&v
> A: 90deg / point

```c
tft.setRotation(3)
> ```
