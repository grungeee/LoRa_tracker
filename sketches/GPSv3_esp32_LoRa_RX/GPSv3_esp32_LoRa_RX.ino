#include "Arduino.h"
#include "LoRa_E220.h"
#include <math.h>

#define FREQUENCY_868
#define DESTINATION_ADDL BROADCAST_ADDRESS
#define TX_PIN 17
#define RX_PIN 16
#define AUX_PIN 4
#define M0_PIN 2
#define M1_PIN 15
LoRa_E220 e220(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);

const int COM_CHAN = 69;

struct GpsData {
    float lat;
    float lon;
    float alt;
    uint8_t sat;
};

const float rxLat = 47.963230;
const float rxLon = 16.196234;
const float rxAlt = 300.0;

double calculateDistance(double lat1, double lon1, double alt1, double lat2, double lon2, double alt2) {
    const double R = 6371000.0;
    double dLat = radians(lat2 - lat1);
    double dLon = radians(lon2 - lon1);
    double a = sin(dLat/2) * sin(dLat/2) + cos(radians(lat1)) * cos(radians(lat2)) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double horizontal = R * c;
    double dAlt = alt2 - alt1;
    return sqrt(horizontal * horizontal + dAlt * dAlt);
}

double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
    double dLon = radians(lon2 - lon1);
    double y = sin(dLon) * cos(radians(lat2));
    double x = cos(radians(lat1)) * sin(radians(lat2)) - sin(radians(lat1)) * cos(radians(lat2)) * cos(dLon);
    double brng = atan2(y, x);
    brng = degrees(brng);
    if (brng < 0) brng += 360.0;
    return brng;
}

void setup() {
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
    if (e220.available() > 1) {
        ResponseStructContainer rsc = e220.receiveMessage(sizeof(GpsData));
        if (rsc.status.code == 1) {
            GpsData data = *(GpsData*)rsc.data;
            double distance = calculateDistance(rxLat, rxLon, rxAlt, data.lat, data.lon, data.alt);
            double bearing = calculateBearing(rxLat, rxLon, data.lat, data.lon);
            Serial.print(F("Lat: ")); Serial.print(data.lat, 6);
            Serial.print(F(" Lon: ")); Serial.print(data.lon, 6);
            Serial.print(F(" Alt: ")); Serial.print(data.alt);
            Serial.print(F(" Sat: ")); Serial.print(data.sat);
            Serial.print(F(" Dist: ")); Serial.print(distance, 1);
            Serial.print(F(" Dir: ")); Serial.println(bearing, 1);
        } else {
            Serial.println(rsc.status.getResponseDescription());
        }
        rsc.close();
    }
}
