#include "GY_BNO055.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x29);

void initBNO() {
    if(!bno.begin()) {
        Serial.println("BNO055 not detected!");
    } else {
        Serial.println("BNO055 initialized");
        bno.setExtCrystalUse(true);
    }
}

float readHeading() {
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    float h = -euler.x();
    if(h < 0) h += 360.0f;
    return h;
}

int headingToLed(float angle) {
    int leds_count = LEDS_NUM;
    float segment = 360.0f / leds_count;
    float shifted = angle + segment / 2.0f;
    int index = ((int)(shifted / segment)) % leds_count;
    index = (index + 1) % leds_count;
    return index + FIRST_LED;
}
