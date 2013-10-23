/*
 * Rocket Telemetry Logger, Rev 2
 *
 * Second revision, based on a ArduIMUv3 and a MPL3115A2 pressure sensor.
 *
 * Data is recorded via an Adafruit MicroSD breakout and transmitted via a
 * Sparkfun XBee Explorer.
 */

#include "MPL3115A2.h"
#include "MPU6000.h"

#define SERIAL_DEBUG

#define RED_LED_PIN 5
#define BLUE_LED_PIN 6

// Timers
unsigned long currentTime;


void setup () {
#ifdef SERIAL_DEBUG
  Serial.begin(38400);  // Note: this has to be set to your GPS speed since they share the same serial port.

  Serial.println();
  Serial.println(F("Rocket Telemetry Logger"));
#endif

  GPS_Init();
  MPU6000_Init();
  MPL3115A2_Init();
  SD_Init();
  XBee_Init();

#ifdef SERIAL_DEBUG
  Serial.println();
#endif

  prettyLights();
}

void loop () {
  if (MPU6000_newdata > 0) {
    MPU6000_newdata = 0;

    currentTime = millis();

    MPL3115A2_Read();
    MPU6000_Read();

#ifdef SERIAL_DEBUG
    Serial.println(stringifyData());
#endif

    SD_Save_Data();
    XBee_Transmit_Data();
  }
}

void serialEvent () {
  GPS_Update();
}

void prettyLights () {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  int count = 4;

  while (count-- >= 0) {
    digitalWrite(RED_LED_PIN, HIGH);
    delay(100);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BLUE_LED_PIN, LOW);
  }
}

String stringifyData () {
  String line;

  line.concat(currentTime);
  line.concat("\t");
  line.concat(int(altitude));
  line.concat("\t");
  line.concat(accelX);
  line.concat("\t");
  line.concat(accelY);
  line.concat("\t");
  line.concat(accelZ);
  line.concat("\t");
  line.concat(gyroX);
  line.concat("\t");
  line.concat(gyroY);
  line.concat("\t");
  line.concat(gyroZ);

  return line;
}

void bufferData (byte  *buf) {
  buf[0] = (byte) currentTime;
  buf[1] = (byte) currentTime >> 8;
  buf[2] = (byte) currentTime >> 16;
  buf[3] = (byte) currentTime >> 24;

  buf[4] = lowByte(int(altitude));
  buf[5] = highByte(int(altitude));

  buf[6] = lowByte(accelX);
  buf[7] = highByte(accelX);

  buf[8] = lowByte(accelY);
  buf[9] = highByte(accelY);

  buf[10] = lowByte(accelZ);
  buf[11] = highByte(accelZ);

  buf[12] = lowByte(gyroX);
  buf[13] = highByte(gyroX);

  buf[14] = lowByte(gyroY);
  buf[15] = highByte(gyroY);

  buf[16] = lowByte(gyroZ);
  buf[17] = highByte(gyroZ);
}

