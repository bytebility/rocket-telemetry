/*
 * Rocket Telemetry, Rev 3
 *
 * Support for Adafriut's 10DOF IMU (http://learn.adafruit.com/adafruit-10-dof-imu-breakout-lsm303-l3gd20-bmp180/).
 *
 * Nominal speedup from messing with the I2C bus speed in /Applications/Arduino.app/Contents/Resources/Java/libraries/Wire/utility/twi.h
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <Adafruit_LSM303_U.h>
#include <Adafruit_L3GD20_U.h>

Adafruit_BMP085_Unified       bmp   = Adafruit_BMP085_Unified(18001);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(30301);
Adafruit_L3GD20_Unified       gyro  = Adafruit_L3GD20_Unified(20);


void TenDOF_Init() {
  Serial1.print("10 DOF... ");

  bmp.begin(BMP085_MODE_STANDARD);
  accel.begin();
  gyro.begin(GYRO_RANGE_2000DPS);

  Serial1.println("done");
}

void Ten_DOF_Update () {
  sensors_event_t event;

  String dataLine;
  dataLine.concat(timeNow);
  dataLine.concat("\t");

  // Altitude
  //
  float altitude = 0.0;
  bmp.getEvent(&event);
  if (event.pressure) {
    float temperature;
    bmp.getTemperature(&temperature);

    // convert to feet
    altitude = bmp.pressureToAltitude(SENSORS_PRESSURE_SEALEVELHPA, event.pressure, temperature) * 3.28084;
  }
  dataLine.concat(int(altitude));
  dataLine.concat("\t");

  // Acceleration
  //
  accel.getEvent(&event);
  enhanceAcceleration(&event);

  // convert to milliG's
  dataLine.concat(int(event.acceleration.x / SENSORS_GRAVITY_EARTH * 1000));
  dataLine.concat("\t");
  dataLine.concat(int(event.acceleration.y / SENSORS_GRAVITY_EARTH * 1000));
  dataLine.concat("\t");
  dataLine.concat(int(event.acceleration.z / SENSORS_GRAVITY_EARTH * 1000));
  dataLine.concat("\t");

  // Gyroscope
  //
  gyro.getEvent(&event);

  // convert to degrees per second
  dataLine.concat(int(event.gyro.x / SENSORS_DPS_TO_RADS));
  dataLine.concat("\t");
  dataLine.concat(int(event.gyro.y / SENSORS_DPS_TO_RADS));
  dataLine.concat("\t");
  dataLine.concat(int(event.gyro.z / SENSORS_DPS_TO_RADS));

  Serial1.println(dataLine);

#ifdef SERIAL_DEBUG
  Serial.println(dataLine);
#endif
}

