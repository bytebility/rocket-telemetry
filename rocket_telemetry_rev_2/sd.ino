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
  if (hasSD) {
    SD_write_line(stringifyData());
  }
}

void SD_Save_NMEA (String line) {
  if (hasSD) {
    SD_write_line(line);
  }
}

void SD_write_line (String line) {
  File dataFile = SD.open(dataFileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(line);
  }
  dataFile.close();
}

