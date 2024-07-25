#include "ESP32_MPU9250.h"
myMPU9250::myMPU9250( int _queueSize )
{
  ESP32_SPI_API _spi;
  _spi.setQSize( _queueSize );
}
int  myMPU9250::getByte( int _Byte)
{
  return _spi.get_rxData8bit( _Byte );
}
int myMPU9250::fInit_MPU9250( bool resetMPU9250, bool resetAK8962 )
{

  SPI_Err = _spi.fSPIbusConfig( );
  SPI_Err = _spi.fSPIdeviceConfig( );
  // select clock source to gyro
  SPI_Err = _spi.fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL );
  vTaskDelay(1);
  // enable I2C master mode
  SPI_Err = _spi.fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN );
  vTaskDelay(1);
  // set the I2C bus speed to 400 kHz
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK );
  vTaskDelay(1);
  // set AK8963 to Power Down
  fWrite_AK8963 (  AK8963_CNTL1, AK8963_PWR_DOWN );
  vTaskDelay(1);
  // reset the MPU9250
  SPI_Err = _spi.fWriteSPIdata8bits( PWR_MGMNT_1, PWR_RESET );
  // wait for MPU-9250 to come back up
  vTaskDelay(1);
  // reset the AK8963 AK8963_CNTL2,AK8963_RESET
  fWrite_AK8963 (  AK8963_CNTL2, AK8963_RESET );
  vTaskDelay(1);
  // select clock source to gyro
  SPI_Err = _spi.fWriteSPIdata8bits( PWR_MGMNT_1, CLOCK_SEL_PLL );
  vTaskDelay(1);
  // enable accelerometer and gyro PWR_MGMNT_2,SEN_ENABLE
  SPI_Err = _spi.fWriteSPIdata8bits( PWR_MGMNT_2, SEN_ENABLE );
  vTaskDelay(1);
  // setting accel range / scale to 16G as default
  setAccelRange( AccellRangeToUse );
  // setting the gyro range / scale to 250DPS as default for calibration
  setGyroRange( GyroRangeToUse );
  // setting bandwidth to 184Hz as default for calibration
  setDlpfBandwidthAccelerometer( DLPF_BANDWIDTH_184HZ );
  // setting bandwidth to 20Hz as default for calibration
  setDlpfBandwidthGyro( DLPF_BANDWIDTH_20HZ );
  // setting the sample rate divider to 19 (0x13) as default
  SPI_Err = _spi.fWriteSPIdata8bits( SMPDIV, 0x13);  // Set sample rate to default of 19
  vTaskDelay(1);
  // enable I2C master mode
  SPI_Err = _spi.fWriteSPIdata8bits( USER_CTRL, I2C_MST_EN );
  vTaskDelay(1);
  // set the I2C bus speed to 400 kHz
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_MST_CTRL, I2C_MST_CLK );
  vTaskDelay(1);
  /* get the magnetometer calibration */
  // set AK8963 to Power Down
  fWrite_AK8963 (  AK8963_CNTL1, AK8963_PWR_DOWN );
  vTaskDelay(100); // long wait between AK8963 mode changes
  // set AK8963 to FUSE ROM access
  fWrite_AK8963 ( AK8963_CNTL1, AK8963_FUSE_ROM );
  vTaskDelay ( 100 ); // delay for mode change
  setMagSensitivityValue( );
  vTaskDelay(1);
  // AK8963_CNTL1,AK8963_CNT_MEAS2
  fWrite_AK8963( AK8963_CNTL1, AK8963_CNT_MEAS2 );
  // delay for mode change
  vTaskDelay ( 100 );
  // AK8963_HXL,7 ;
  fReadAK8963(AK8963_HXL, 7 | SPI_READ );
  vTaskDelay(1);
  fCalculate_GandAbias();
}void myMPU9250::fWrite_AK8963(  int subAddress, int dataAK8963 )
{
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_SLV0_ADDR, AK8963_I2C_ADDR );
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_SLV0_REG, subAddress );
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_SLV0_DO, dataAK8963 );
  SPI_Err = _spi.fWriteSPIdata8bits( I2C_SLV0_CTRL, I2C_SLV0_EN | (uint8_t)1 );
}
int myMPU9250::setDlpfBandwidthAccelerometer( int bandwidth )
{
  SPI_Err = 0;
  if ( bandwidth == DLPF_BANDWIDTH_184HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_184);
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_92HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_92 );
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_41HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_41 );
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_20HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_20 );
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_10HZ )
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_10 );
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_5HZ )
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)ACCEL_CONFIG2, (int)ACCEL_DLPF_5 );
    vTaskDelay(1);
    a_bandwidth = bandwidth;
    return SPI_Err;
  }
} // int setDlpfBandwidth(DlpfBandwidth bandwidth
int myMPU9250::setDlpfBandwidthGyro( int bandwidth )
{
  SPI_Err = 0;
  if ( bandwidth == DLPF_BANDWIDTH_184HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_184);
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_92HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_92 );
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_41HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_41 );
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_20HZ)
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_20 );
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_10HZ )
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_10 );
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
  if ( bandwidth == DLPF_BANDWIDTH_5HZ )
  {
    SPI_Err = _spi.fWriteSPIdata8bits( (int)CONFIG, (int)GYRO_DLPF_5 );
    vTaskDelay(1);
    g_bandwidth = bandwidth;
    return SPI_Err;
  }
}
