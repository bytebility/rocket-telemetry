/*
 * Rocket Telemetry, Rev 3
 *
 * Support for the Adafruit ADXL377 breakout (http://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/).
 */

const int xInput = A0;
const int yInput = A1;
const int zInput = A2;
const int sampleSize = 10;


void ADXL_Init () {
  Serial1.println("ADXL377... done");
}

int ADXL_ReadXAccel () {
  int raw = ADXL_ReadAxis(xInput) - 1;
  return map (raw, 0, 1023, -1962, 1962);
}

int ADXL_ReadYAccel () {
  int raw = ADXL_ReadAxis(yInput) - 0;
  return map (raw, 0, 1023, -1962, 1962);
}

int ADXL_ReadZAccel () {
  int raw = ADXL_ReadAxis(zInput) - 0;
  return map (raw, 0, 1023, -1962, 1962);
}

int ADXL_ReadAxis (int axisPin) {
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < sampleSize; i++) {
    reading += analogRead(axisPin);
  }
  return reading/sampleSize;
}

void enhanceAcceleration (sensors_event_t *event) {
  // 152 m/s^2 is ~15.5 G's
  if (event->acceleration.x > 152 || event->acceleration.x < -152) {
    event->acceleration.x = ADXL_ReadXAccel();
  }
  if (event->acceleration.y > 152 || event->acceleration.y < -152) {
    event->acceleration.y = ADXL_ReadYAccel();
  }
  if (event->acceleration.z > 152 || event->acceleration.z < -152) {
    event->acceleration.z = ADXL_ReadZAccel();
  }
}
