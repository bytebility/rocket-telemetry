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


void setup() {
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

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  prettyLights();
}


void loop () {
  if (MPU6000_newdata > 0) {
    MPU6000_newdata = 0;

    currentTime = millis();

    MPL3115A2_Read();
    MPU6000_Read();

    SD_Save_Data();
    XBee_Transmit_Data();
  }
}

void serialEvent() {
  GPS_Update();
}

void prettyLights() {
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

