#include "Arduino.h"
#include "LoRa_E220.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 17
#define RX_PIN 16
#define AUX_PIN 4
#define M0_PIN 2
#define M1_PIN 15
LoRa_E220 e220(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

const int RXPinGPS = 33;
const int TXPinGPS = 32;
const uint32_t GPSBaud = 9600;
const int COM_CHAN = 69;

struct GpsData {
    float lat;
    float lon;
    float alt;
    uint8_t sat;
};

void setup() {
    gpsSerial.begin(GPSBaud, SERIAL_8N1, RXPinGPS, TXPinGPS);
    Serial.begin(115200);

    e220.begin();
    ResponseStructContainer c = e220.getConfiguration();
    Configuration config = *(Configuration*)c.data;
    config.ADDL = BROADCAST_ADDRESS;
    config.ADDH = BROADCAST_ADDRESS;
    config.CHAN = COM_CHAN;
    config.SPED.uartBaudRate = UART_BPS_9600;
    config.SPED.airDataRate = AIR_DATA_RATE_010_24;
    config.SPED.uartParity = MODE_00_8N1;
    config.OPTION.subPacketSetting = SPS_200_00;
    config.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
    config.OPTION.transmissionPower = POWER_22;
    config.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
    config.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
    config.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
    config.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
    e220.setConfiguration(config, WRITE_CFG_PWR_DWN_SAVE);
    c.close();
}

void loop() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isUpdated()) {
        GpsData data;
        data.lat = gps.location.lat();
        data.lon = gps.location.lng();
        data.alt = gps.altitude.meters();
        data.sat = gps.satellites.value();

        ResponseStatus rs = e220.sendFixedMessage(0, DESTINATION_ADDL, COM_CHAN, &data, sizeof(GpsData));
        Serial.print(F("Send status: ")); Serial.println(rs.getResponseDescription());
        delay(1000);
    }
}
