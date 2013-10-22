/*
 * Rocket Telemetry, Rev 2
 *
 * XBee radio support.
 */

#include <SoftwareSerial.h>

/*
 * Sets the broadcast interval in ms.  It's really easy to overload your
 * receiver if you send data too fast.  Anything over than about 10hz can get
 * problematic.
 */
#define BROADCAST_INTERVAL 120

/*
 * Sets the baud rate.  MAke sure you've set your XBee cards to transmit at
 * this speed.
 *
 * Standard 2.4ghz XBee cards can transmit at 250kbps, while the 900mhz cards
 * can transmit at 156kbps, both well above Arduino's 115.2kpbs upper limit.
 */
#define BAUD_RATE 57600


SoftwareSerial radio(8, 9);  // RX, TX

unsigned long lastBroadcastTime = 0;


void XBee_Init() {
  // Make sure your XBee radios are set to this speed!
  radio.begin(BAUD_RATE);

#ifdef SERIAL_DEBUG
  Serial.println(F("XBee initialization successful"));
#endif
}

void XBee_Transmit_Data () {
  if ((currentTime - lastBroadcastTime) < BROADCAST_INTERVAL) {
    return;
  }

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

  lastBroadcastTime = currentTime;
}

void XBee_Transmit_NMEA (String line) {
  radio.println(line);
}
