/*
 * Rocket Telemetry Logger, Rev 3
 *
 * Third revision, based on a Sparkfun Fio v3 along with several modules from
 * Adafruit, including an Adafruit 10-DOF IMU Breakout, an ADXL377 and an
 * Adafruit Ultimate GPS Breakout .
 *
 * Data is recorded via an Adafruit MicroSD breakout and transmitted via an
 * XBee radio.
 */

#include <Adafruit_Sensor.h>

//#define SERIAL_DEBUG
//#define ENABLE_SD   // (~9.7k)
#define ENABLE_GPS  // (~2.4k)

long timeNow = 0;
long timeLast = 0;

void setup () {
#ifdef SERIAL_DEBUG
  Serial.begin(115200);
#endif

  Serial1.begin(115200);
  
  TenDOF_Init();
  ADXL_Init();

#ifdef ENABLE_GPS
  GPS_Init();
#endif

#ifdef ENABLE_SD
  SD_Init();
#endif
}

void loop () {
  timeNow = millis();
  
  // Note that running much higher than 10hz overrun the XBee reciever
  if ((timeNow - timeLast) >= 100) {  // Run at 10hz
    timeLast = timeNow;
    Ten_DOF_Update();
  }
  
#ifdef ENABLE_GPS
  GPS_Update();
#endif
}

