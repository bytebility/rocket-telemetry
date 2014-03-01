/*
 * Rocket Telemetry, Rev 3
 *
 * Support for the Adafruit Ultimate GPS Breakout (http://learn.adafruit.com/adafruit-ultimate-gps/).
 *
 * You probably need to increase the SoftwareSerial RX buffer 64 -> 256 at:
 * /Applications/Arduino.app/Contents/Resources/Java/libraries/SoftwareSerial/SoftwareSerial.h
 */

#ifdef ENABLE_GPS

#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

SoftwareSerial gpsSerial(9, 10); // RX, TX
Adafruit_GPS GPS(&gpsSerial);


void GPS_Init () {
  Serial1.print("GPS... ");
  
   // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  
  //GPS.sendCommand("$PMTK251,38400*27");
  //GPS.begin(38400);
  //GPS.sendCommand(PMTK_SET_BAUD_57600);
  //GPS.begin(57600);

  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

  // Only get the GGA and GSA messages
  GPS.sendCommand("$PMTK314,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28");
  
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

  // This is a little more voodoo than I grok at present.  Timer0 stuff lifted
  // from the Adafruit_GPS "parsing" example.
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  delay(1000);

  Serial1.println("done");
}

void GPS_Update () {
  if (GPS.newNMEAreceived()) {
    String nmeaString = GPS.lastNMEA();
    nmeaString.trim();

    Serial1.println(nmeaString);
    
#ifdef SERIAL_DEBUG
    Serial.println(nmeaString);
#endif

#ifdef ENABLE_SD
    SD_Save_Data(nmeaString);
#endif
  }
}

SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  
#ifdef SERIAL_DEBUG
  if (gpsSerial.overflow()) {
    Serial.println("### SoftwareSerial overflow! ###");
  }
#endif
}

#endif
