/*
 * Rocket Telemetry, Rev 2
 *
 * Support for a GP-635T GPS Receiver (https://www.sparkfun.com/products/11571).
 */

#define SERIAL_MUX_PIN 7

String nmeaString = "";

void GPS_Init () {
#ifdef SERIAL_DEBUG
  Serial.print(F("Configuring GPS... "));
  Serial.flush();
#endif

  Serial.begin(9600);  // The GPS defaults to 9600, config code will change this

  pinMode(SERIAL_MUX_PIN, OUTPUT);
  digitalWrite(SERIAL_MUX_PIN, HIGH);

  gpsConfig();

#ifdef SERIAL_DEBUG
  Serial.println(F(" done"));
#endif
}

void GPS_Update () {
  while (Serial.available()) {
    readGps();
  }
}

/*
 * Read serial data coming from the GPS.  NMEA messages start with a "$" and
 * end with a "\n" so we can use those to start or end strings.
 */
void readGps () {
  char gpsChar = Serial.read();

  switch (gpsChar) {
    case '$':
      // The "$" signals the start of a NMEA message.  So clear the saved
      // nmeaString var and start it with the "$".
      nmeaString = "$";
      break;
    case '\r':
      // Ignore CR's
      break;
    case '\n':
      // A LF means we're at the end of a NMEA message, so do some basic
      // verification (starts with $GP and is greater than $GPxxx length) and
      // then do something with it.
      //
      // Clear the saved nmeaString var either way.
      if (nmeaString.length() > 6 && nmeaString.startsWith("$GP")) {

#ifdef SERIAL_DEBUG
        debug_NMEA();
#endif

        SD_Save_NMEA(nmeaString);

        if (nmeaString.startsWith("$GPGGA") || nmeaString.startsWith("$GPGSA"))
          XBee_Transmit_NMEA(nmeaString);
      }

      nmeaString = "";
      break;
    default:
      nmeaString.concat(gpsChar);
  }
}

#ifdef SERIAL_DEBUG
void debug_NMEA () {
  // Don't re-print the full NMEA line wih the $ to the Serial console,
  // the GPS will interpret them as commands.
  String SerialSafeLine = nmeaString;
  SerialSafeLine.replace("$", "");
  Serial.println(SerialSafeLine);
}
#endif

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
  byte ubxCfgRate[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xF4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77
  };
  sendUBX(&ubxCfgRate[0], sizeof(ubxCfgRate));

  // Sets the baudrate to 19200
  //byte ubxCfgPrt[] = {
  //  0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0x4B,
  //  0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x44, 0x37
  //};
  //sendUBX(&ubxCfgPrt[0], sizeof(ubxCfgPrt));
  //Serial.begin(19200);

  // Sets the baudrate to 38400
  byte ubxCfgPrt[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0x96,
    0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8F, 0x70
  };
  sendUBX(&ubxCfgPrt[0], sizeof(ubxCfgPrt));
  Serial.begin(38400);

  // Sets the baudrate to 56700
  //byte ubxCfgPrt[] = {
  //  0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00, 0x00, 0xE1,
  //  0x00, 0x00, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDA, 0xA9
  //};
  //sendUBX(&ubxCfgPrt[0], sizeof(ubxCfgPrt));
  //Serial.begin(56700);
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

