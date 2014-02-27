/*
 * Rocket Telemetry, Rev 3
 *
 * SD device support.
 */

#ifdef ENABLE_SD

#include <SdFat.h>

SdFat sd;
SdFile dataFile;
ofstream logfile;

const int chipSelect = 17;
// buffer to format data - makes it eaiser to echo to Serial
char buf[80];


void SD_Init () {
  Serial1.print("SD... ");

  if (sd.begin(chipSelect, SPI_HALF_SPEED)) {
    char name[] = "DATA-00.TXT";
    
    for (uint8_t i = 0; i < 100; i++) {
      name[5] = i/10 + '0';
      name[6] = i%10 + '0';
      if (sd.exists(name)) continue;
      logfile.open(name);
      Serial1.print(name);
      break;
    }
    if (logfile.is_open()) {
      Serial1.println("done");
    } else {
      Serial1.println("file error");
    }
  } else {
    Serial1.println("no SD card");
  }
}

void SD_Save_Data (String line) {
  if (logfile.is_open()) {
    char buf[80];
    line.concat("\n");
    line.toCharArray(buf, sizeof(buf));
    logfile << buf << flush;
  }
}

#endif
