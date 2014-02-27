Rocket Telemetry Logger, Rev. 3
----

The third revision of my rocket telemetry rig is built around a [Sparkfun Fio v3](https://www.sparkfun.com/products/11520).  The Fio is a 3.3v, LiPo ready, ATmega32U4-based Arduino that also sports an on-board XBee socket which will hold an [XBee Pro 900 Wire Antenna](https://www.sparkfun.com/products/9097) module for ground communication.

Sensors include an I2C-based [Adafruit 10-DOF IMU Breakout](https://www.adafruit.com/products/1604) which itself includes an LSM303 3-axis accelerometer (±16g), a L3GD20 Gyro (±2000 deg/sec), a LSM303 compass (which isn't used) and a BMP180 altimeter all in one package.

An additional analog [Adafruit ADXL377 - High-G Triple-Axis Accelerometer](https://www.adafruit.com/products/1413) is included to handle high-G acceleration data.  This is going in a rocket after all.

Also included is an [Adafruit Ultimate GPS Breakout](https://www.adafruit.com/products/746) for serial GPS data.

Finally an SPI-based [Adafruit MicroSD board](https://www.adafruit.com/products/254) is available for saving raw data at the source.

-30-
