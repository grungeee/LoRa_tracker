#include "E220.h"

LoRa_E220 e220(RX_PIN, TX_PIN, &Serial2, AUX_PIN, M0_PIN, M1_PIN, UART_BPS_RATE_9600);

void initE220() {
  e220.begin();
  Serial.println("E220 module initialized");
}

void configE220() {
  ResponseStructContainer c;
  c = e220.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;

  configuration.ADDL = BROADCAST_ADDRESS;
  configuration.ADDH = BROADCAST_ADDRESS;
  configuration.CHAN = 69;
  configuration.SPED.uartBaudRate = UART_BPS_9600;
  configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
  configuration.SPED.uartParity = MODE_00_8N1;
  configuration.OPTION.subPacketSetting = SPS_200_00;
  configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
  configuration.OPTION.transmissionPower = POWER_22;
  configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
  configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
  configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
  configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;

  e220.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
  c.close();

  Serial.println("E220 module configured");
}

void sendE220Message(int channel, const String& message) {
  ResponseStatus rs = e220.sendBroadcastFixedMessage(comChan, message);
  //Serial.println(rs.getResponseDescription());
}


// not sure if i did it right
void gpsDataRX(){
    if (e220.available() > 1) {
        ResponseStructContainer rsc = e220.receiveMessage(sizeof(GpsData));
        if (rsc.status.code == 1) {
            GpsData data = *(GpsData*)rsc.data;
            Serial.print(F("Lat: ")); Serial.print(data.lat, 6);
            Serial.print(F(" Lon: ")); Serial.print(data.lon, 6);
            Serial.print(F(" Alt: ")); Serial.print(data.alt);
            Serial.print(F(" Sat: ")); Serial.println(data.sat);
        } else {
            Serial.println(rsc.status.getResponseDescription());
        }
        rsc.close();
    }
}


// Todo: implement RSSI at some point

void receiveE220Message() {
	// If something available
  if (e220.available() > 1) {
	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220tt.receiveMessageRSSI();
#else
	ResponseContainer rc = e220.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
	//	Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
  }
}
}

/*
*/




/*
  // |================< E220 Communication >==================|
	// If something available
  if (e220.available()>1) {
	  // read the String message
#ifdef ENABLE_RSSI
	ResponseContainer rc = e220tt.receiveMessageRSSI();
#else
	ResponseContainer rc = e220.receiveMessage();
#endif
	// Is something goes wrong print error
	if (rc.status.code!=1){
		Serial.println(rc.status.getResponseDescription());
	}else{
		// Print the data received
		Serial.println(rc.status.getResponseDescription());
		Serial.println(rc.data);
#ifdef ENABLE_RSSI
		Serial.print("RSSI: "); Serial.println(rc.rssi, DEC);
#endif
	}
  }
  if (Serial.available()) {
	  String input = Serial.readString();
	  ResponseStatus rs = e220.sendBroadcastFixedMessage(69, input);
	  // Check If there is some problem of succesfully send
	  Serial.println(rs.getResponseDescription());
  }

*/



//	----------------------- DEFAULT TRANSPARENT WITH RSSI -----------------------
//	configuration.ADDL = 0x03;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
// ResponseStatus rs = e220.setConfiguration(configuration);
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_ENABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
//	----------------------- FIXED SENDER -----------------------
//	configuration.ADDL = 0x02;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
//
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
//
//	----------------------- FIXED RECEIVER -----------------------
//	configuration.ADDL = 0x03;
//	configuration.ADDH = 0x00;
//
//	configuration.CHAN = 23;
//
//	configuration.SPED.uartBaudRate = UART_BPS_9600;
//	configuration.SPED.airDataRate = AIR_DATA_RATE_010_24;
//	configuration.SPED.uartParity = MODE_00_8N1;
//
//	configuration.OPTION.subPacketSetting = SPS_200_00;
//	configuration.OPTION.RSSIAmbientNoise = RSSI_AMBIENT_NOISE_DISABLED;
//	configuration.OPTION.transmissionPower = POWER_22;
//
//	configuration.TRANSMISSION_MODE.enableRSSI = RSSI_DISABLED;
//	configuration.TRANSMISSION_MODE.fixedTransmission = FT_FIXED_TRANSMISSION;
//	configuration.TRANSMISSION_MODE.enableLBT = LBT_DISABLED;
//	configuration.TRANSMISSION_MODE.WORPeriod = WOR_2000_011;
