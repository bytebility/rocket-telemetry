/*
 * Rocket Telemetry Logger, Rev 1
 *
 * Hardware:
 *   Arduino UNO + Seeed SD Shield
 *
 * Sensors:
 *   On the I2C bus (pins A4,A5)
 *     - BMP-085 barometric pressure sensor
 *     - ADXL345 accelerometer
 *
 *   On UART Serial
 *     - GPS
 *
 *   On SPI (pins 10,11,12,13)
 *    - SD Slot (on the Seeed SD Shield)
 *
 * Notes:
 *   Uses I2Cdev library collection, https://github.com/jrowberg/i2cdevlib
 */

#include <Wire.h>
#include <I2Cdev.h>
#include <BMP085.h>
#include <ADXL345.h>
#include <SD.h>


BMP085 barometer;
ADXL345 accelerometer;

boolean hasBmp;
boolean hasAdxl;
char dataFileName[] = "DATA-00.TXT";
String gpsString = "";
float altitude = 0;
int ax = 0, ay = 0, az = 0;
float accel_resolution = 0.0039;  // 3.9 mg/LSB at full resolution


void setup() {
  Wire.begin();
  Serial.begin(9600);

  // init SD and create a new file...
  // SD is basically required since the GPS uses the serial UART
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    while (1) {}
  }
  int dataFileIndex = 0;
  while (SD.exists(dataFileName)) {
    sprintf(dataFileName, "DATA-%02d.TXT", ++dataFileIndex);
  }

  Serial.println("Initializing I2C devices...");

  // init barometer...
  barometer.initialize();
  hasBmp = barometer.testConnection();

  // init accelerometer...
  accelerometer.initialize();
  hasAdxl = accelerometer.testConnection();
  accelerometer.setFullResolution(1);
  accelerometer.setRange(3);
  //accelerometer.setOffset(250, 3, 10);

  // init GPS...
  Serial.println("Initializing GPS...");
  gpsConfig();
}


void loop () {
  // Altitude
  if (hasBmp)
    altitude = getAltitude();

  // Acceleration
  if (hasAdxl)
    accelerometer.getAcceleration(&ax, &ay, &az);

  // Write it to the SD card
  File dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.print(millis());
    dataFile.print("\t");

    dataFile.print(altitude);
    dataFile.print("\t");

    dataFile.print(ax * accel_resolution);
    dataFile.print("\t");
    dataFile.print(ay * accel_resolution);
    dataFile.print("\t");
    dataFile.println(az * accel_resolution);

    dataFile.close();
  }

  /*
   * Read serial data coming from the GPS.  NMEA messages start with a "$" and
   * end with a "\n" so we can use those to start or end strings.
   */
  while (Serial.available()) {
    char gpsChar = Serial.read();

    if (gpsChar == '$')
      gpsString = "";

    if (gpsChar == '\n' && gpsString.length() > 0) {
      File dataFile = SD.open(dataFileName, FILE_WRITE);
      if (dataFile) {
        dataFile.println(gpsString);
        dataFile.close();
      }
      gpsString = "";
    }

    if (gpsChar != '\r')
      gpsString.concat(gpsChar);
  }
}

/*
 * Get the Altitude.
 *
 * The I2Cdev library doesn't do all the steps necessary to finagle a BMP085 to regugitate the altitude,
 * so we have to help ot along.
 */
float getAltitude () {
  float temperature;
  float pressure;
  float altitude;

  barometer.setControl(BMP085_MODE_TEMPERATURE);
  delayMicroseconds(barometer.getMeasureDelayMicroseconds());
  temperature = barometer.getTemperatureC();

  barometer.setControl(BMP085_MODE_PRESSURE_1);
  delayMicroseconds(barometer.getMeasureDelayMicroseconds());  // Note: delayMicroseconds will not delay long enough for BMP085_MODE_PRESSURE_3
  pressure = barometer.getPressure();

  altitude = barometer.getAltitude(pressure);

  return altitude * 3.28084;
}

/*
 * GPS Configuration.
 * UBX bytes are from u-center.
 * u-cenetr: http://www.u-blox.com/en/evaluation-tools-a-software/u-center/u-center.html
 *
 * Ideas at:
 *     - https://code.google.com/p/ardupirates/wiki/UbloxGPSTutorial
 *     - http://playground.arduino.cc/UBlox/GPS
 *
 * Note: We're basically sending these things "blind", not waiting for a
 *       UBX_ACK response at all.
 */
void gpsConfig () {
  // Set nav5 mode to "Airborne < 4G" Mode
  byte ubxCfgNav5[] = {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x08, 0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27,
    0x00, 0x00, 0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x20
  };
  sendUBX(&ubxCfgNav5[0], sizeof(ubxCfgNav5));

  // Sets the rate to 2Hz
  byte ubxCfgRate[] = { 0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xF4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77 };
  sendUBX(&ubxCfgRate[0], sizeof(ubxCfgRate));
}

/*
 * Helper to actually send UBX messages over serial.
 */
void sendUBX(byte *UBXmsg, byte msgLength) {
  for (int i = 0; i < msgLength; i++) {
    Serial.write(UBXmsg[i]);
    Serial.flush();
  }

  Serial.println();
  Serial.flush();
}
