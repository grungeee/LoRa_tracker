#include "MPU9250.h"

MPU9250 mpu; // You can also use MPU9255 as is
// --------------------------
// ---- I2C
//MPU9250 mpu(Wire, 0x68);
//mpu.setup();

// ---- SPI
#define csPin 
MPU9250 mpu;
mpu.setup(csPin); // Specify the CS pin if it is not the default
// --------------------------
void setup() {
    Serial.begin(115200);
    Wire.begin();
    delay(2000);

    mpu.setup(0x68);  // change to your own address
}

void loop() {
    if (mpu.update()) {
        Serial.print(mpu.getYaw()); Serial.print(", ");
        Serial.print(mpu.getPitch()); Serial.print(", ");
        Serial.println(mpu.getRoll());
    }
}
