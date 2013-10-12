/*
 * Rocket Telemetry, Rev 2
 *
 * XBee radio support.
 */

#include <SoftwareSerial.h>

SoftwareSerial radio(8, 9);  // RX, TX

void XBee_Init() {
  // Make sure your XBee radios are set to this speed!
  radio.begin(57600);

#ifdef SERIAL_DEBUG
  Serial.println(F("XBee initialization successful"));
#endif
}

void XBee_Transmit_Data () {
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

  radio.println(line);
}

void XBee_Transmit_NMEA (String line) {
  radio.println(line);
}
