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
unsigned long lastTime;
unsigned long interval;
unsigned int cycleCount = 0;


void setup() {
#ifdef SERIAL_DEBUG
  Serial.begin(38400);
  
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
  currentTime = millis();
  interval = currentTime - lastTime;
  
  if (interval >= 20) {  // every 20 milliseconds aka 50 Hz
    MPL3115A2_Read();
    MPU6000_Read();

    SD_Save_Data();

    // Do these things every 6th time through the main cycle
    // This section gets called every 1000 / 20 * 6 = 8 1/3 Hz or every 120 millisecods.
    // Doing it this way removes the need for another 'millis()' call
    // and balances the processing load across main loop cycles.
    cycleCount++;
    switch (cycleCount) {
      case(0):
        break;
      case(1):
        XBee_Transmit_Data();
        break;
      case(2):
        break;
      case(3):
        break;
      case(4):
        break;
      case(5):
        cycleCount = -1;
        break;
    }
    
    lastTime = currentTime;
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

