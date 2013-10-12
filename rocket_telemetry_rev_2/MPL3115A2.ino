/*
 * Support for a MPL3115A2 for getting Altitude.
 *
 * See: 
 * https://github.com/sparkfun/MPL3115A2_Breakout/blob/master/firmware/MPL3115A2/MPL3115A2.ino
 */

#include <Wire.h>

boolean hasMPL = false;

boolean MPL3115A2_Init () {
  Wire.begin();
  
  if (IIC_Read(WHO_AM_I) == 196) {
    // Configure the sensor
    setModeAltimeter(); // Measure altitude above sea level in meters
  
    setOversampleRate(128); // Set Oversample to the recommended 128
    enableEventFlags(); // Enable all three pressure and temp event flags 
    setModeActive(); // Go to active mode and start measuring!

    hasMPL = true;
  }
  
#ifdef SERIAL_DEBUG
  if (hasMPL) {
    Serial.println(F("MPL3115A2 initialization successful"));
  } else {
    Serial.println(F("MPL3115A2 connection failed"));
  }
#endif

  return hasMPL;
}

//Sets the mode to Altimeter
//CTRL_REG1, ALT bit
void setModeAltimeter()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting |= (1<<7); //Set ALT bit
  IIC_Write(CTRL_REG1, tempSetting);
}

//Call with a rate from 0 to 7. See page 33 for table of ratios.
//Sets the over sample rate. Datasheet calls for 128 but you can set it 
//from 1 to 128 samples. The higher the oversample rate the greater
//the time between data samples.
void setOversampleRate(byte sampleRate)
{
  if(sampleRate > 7) sampleRate = 7; //OS cannot be larger than 0b.0111
  sampleRate <<= 3; //Align it for the CTRL_REG1 register
  
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting &= 0b11000111; //Clear out old OS bits
  tempSetting |= sampleRate; //Mask in new OS bits
  IIC_Write(CTRL_REG1, tempSetting);
}

//Enables the pressure and temp measurement event flags so that we can
//test against them. This is recommended in datasheet during setup.
void enableEventFlags()
{
  IIC_Write(PT_DATA_CFG, 0x07); // Enable all three pressure and temp event flags 
}

//Puts the sensor in active mode
//This is needed so that we can modify the major control registers
void setModeActive()
{
  byte tempSetting = IIC_Read(CTRL_REG1); //Read current settings
  tempSetting |= (1<<0); //Set SBYB bit for Active mode
  IIC_Write(CTRL_REG1, tempSetting);
}

//Returns the number of meters above sea level
float readAltitude()
{
  // New data = wait for PDR (bit 2) to be set
  while( (IIC_Read(STATUS) & (1<<2)) == 0) break; // If PDR bit is set then we have new pressure data

  // Read pressure registers
  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(OUT_P_MSB);  // Address of data to get
  Wire.endTransmission(false); // Send data to I2C dev with option for a repeated start. THIS IS NECESSARY and not supported before Arduino V1.0.1!
  Wire.requestFrom(MPL3115A2_ADDRESS, 3); // Request three bytes

  //Wait for data to become available
  int counter = 0;
  while(Wire.available() < 3)
  {
    if(counter++ > 100) return 0; //Error out
    delay(1);
  }

  byte msb, csb, lsb;
  msb = Wire.read();
  csb = Wire.read();
  lsb = Wire.read();

  // The least significant bytes l_altitude and l_temp are 4-bit,
  // fractional values, so you must cast the calulation in (float),
  // shift the value over 4 spots to the right and divide by 16 (since 
  // there are 16 values in 4-bits). 
  float tempcsb = (lsb>>4)/16.0;

  float altitude = (float)( (msb << 8) | csb) + tempcsb;

  return(altitude);
}

float readAltitudeFt()
{
  return(readAltitude() * 3.28084);
}

void MPL3115A2_Read () {
  altitude = readAltitudeFt();
}


byte IIC_Read(byte regAddr)
{
  // This function reads one byte over IIC
  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(regAddr);  // Address of CTRL_REG1
  Wire.endTransmission(false); // Send data to I2C dev with option
                               //  for a repeated start. THIS IS
                               //  NECESSARY and not supported before
                               //  Arduino V1.0.1!!!!!!!!!
  Wire.requestFrom(MPL3115A2_ADDRESS, 1); // Request the data...
  return Wire.read();
}

void IIC_Write(byte regAddr, byte value)
{
  // This function writes one byto over IIC
  Wire.beginTransmission(MPL3115A2_ADDRESS);
  Wire.write(regAddr);
  Wire.write(value);
  Wire.endTransmission(true);
}
