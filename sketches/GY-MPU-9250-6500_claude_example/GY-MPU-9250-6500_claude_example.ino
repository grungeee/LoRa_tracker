#include <Wire.h>

#define MPU9250_ADDR 0x68
#define MAG_ADDR 0x0C

// normal esp32
#define SDA_PIN 21  // Default SDA pin for ESP32
#define SCL_PIN 22  // Default SCL pin for ESP32
// esp32-c3
//#define SDA_PIN 8  // Default SDA pin for ESP32
//#define SCL_PIN 9  // Default SCL pin for ESP32

float accelX, accelY, accelZ;
float gyroX, gyroY, gyroZ;
float magX, magY, magZ;

void setup() {
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.begin(115200);
  
  // Initialize MPU9250
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // Wake up the MPU-9250
  Wire.endTransmission(true);
  
  // Configure accelerometer
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x1C);  // ACCEL_CONFIG register
  Wire.write(0x00);  // Set range to ±2g
  Wire.endTransmission(true);
  
  // Configure gyroscope
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x1B);  // GYRO_CONFIG register
  Wire.write(0x00);  // Set range to ±250 degrees/second
  Wire.endTransmission(true);
  
  // Enable I2C bypass to access magnetometer
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x37);  // INT_PIN_CFG register
  Wire.write(0x02);  // Enable I2C bypass
  Wire.endTransmission(true);
  
  // Configure magnetometer
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x0A);  // CNTL1 register
  Wire.write(0x16);  // Set to continuous measurement mode 2 (100 Hz)
  Wire.endTransmission(true);
  
  delay(100); // Wait for magnetometer to initialize
}

void loop() {
  // Read accelerometer data
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x3B);  // Starting register for accel readings
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDR, 6, true);  // Request 6 bytes
  
  accelX = (Wire.read() << 8 | Wire.read()) / 16384.0; // Divide by 16384 for ±2g range
  accelY = (Wire.read() << 8 | Wire.read()) / 16384.0;
  accelZ = (Wire.read() << 8 | Wire.read()) / 16384.0;

  // Read gyroscope data
  Wire.beginTransmission(MPU9250_ADDR);
  Wire.write(0x43);  // Starting register for gyro readings
  Wire.endTransmission(false);
  Wire.requestFrom(MPU9250_ADDR, 6, true);  // Request 6 bytes
  
  gyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // Divide by 131 for ±250 degrees/second range
  gyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  gyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;

  // Read magnetometer data
  Wire.beginTransmission(MAG_ADDR);
  Wire.write(0x02);  // Status register 1
  Wire.endTransmission(false);
  Wire.requestFrom(MAG_ADDR, 1);
  
  if(Wire.read() & 0x01) {  // Check if data is ready
    Wire.beginTransmission(MAG_ADDR);
    Wire.write(0x03);  // Starting register for mag readings
    Wire.endTransmission(false);
    Wire.requestFrom(MAG_ADDR, 7, true);  // Request 7 bytes (6 data + 1 status)
    
    int16_t magX_raw = Wire.read() | Wire.read() << 8;
    int16_t magY_raw = Wire.read() | Wire.read() << 8;
    int16_t magZ_raw = Wire.read() | Wire.read() << 8;
    uint8_t status2 = Wire.read();
    
    if(!(status2 & 0x08)) {  // Check if magnetic sensor overflow occurred
      magX = magX_raw * 0.15;  // Multiply by 0.15 for 4900 uT range
      magY = magY_raw * 0.15;
      magZ = magZ_raw * 0.15;
    } else {
      Serial.println("Magnetic sensor overflow occurred");
    }
  }

  // Print the data
  Serial.print("Accel (g): ");
  Serial.print(accelX); Serial.print(", ");
  Serial.print(accelY); Serial.print(", ");
  Serial.println(accelZ);

  Serial.print("Gyro (deg/s): ");
  Serial.print(gyroX); Serial.print(", ");
  Serial.print(gyroY); Serial.print(", ");
  Serial.println(gyroZ);

  Serial.print("Mag (uT): ");
  Serial.print(magX); Serial.print(", ");
  Serial.print(magY); Serial.print(", ");
  Serial.println(magZ);

  Serial.println();

  delay(100);  // Delay for readability
}
