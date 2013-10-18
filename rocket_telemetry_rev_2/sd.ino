/*
 * Rocket Telemetry, Rev 2
 *
 * SD device support.
 */

#include <SD.h>

boolean hasSD = false;
char dataFileName[] = "DATA-00.TXT";

void SD_Init () {
#ifdef SERIAL_DEBUG
  Serial.print(F("Checking for SD device... "));
#endif

  if (SD.begin()) {
    int dataFileIndex = 0;
    while (SD.exists(dataFileName)) {
      sprintf(dataFileName, "DATA-%02d.TXT", ++dataFileIndex);
    }
    hasSD = true;
  }

#ifdef SERIAL_DEBUG
  if (hasSD) {
    Serial.println(F("SD card detected"));
  } else {
    Serial.println(F("NO SD card detected"));
  }
#endif
}

void SD_Save_Data () {
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

#ifdef SERIAL_DEBUG
  Serial.println(line);
#endif

  SD_write_line(line);
}

void SD_Save_NMEA (String line) {
#ifdef SERIAL_DEBUG
  // Don't re-print the full NMEA line wih the $ to the Serial console,
  // the GPS will interpret them as commands.
  String SerialSafeLine = line;
  SerialSafeLine.replace("$", "");
  Serial.println(SerialSafeLine);
#endif

  SD_write_line(line);
}

void SD_write_line (String line) {
  if (hasSD) {
    File dataFile = SD.open(dataFileName, FILE_WRITE);
    if (dataFile) {
      dataFile.println(line);
    }
    dataFile.close();

    // Make sure the SS pin for the SD card reader is set back to high to ensure 
    // theres no crosstalk over the MPU6000
    digitalWrite(10, HIGH);
  }
}

