Rocket Telemetry Logger, Rev. 1
----

The first revision of my telemetry rig, built on an [Arduino UNO](http://arduino.cc/en/Main/ArduinoBoardUno) with a [Seeed SD shield (v3)](http://www.seeedstudio.com/depot/sd-card-shield-p-492.html) on top.  As seen in this [YouTube video](https://www.youtube.com/watch?v=rpGxVXfmJk8).

It includes an [ADXL345 accelerometer](http://www.seeedstudio.com/depot/grove-3axis-digital-accelerometer16g-p-1156.html), a [BMP085 barometer](http://www.seeedstudio.com/depot/grove-barometer-sensor-p-1199.html) and a [u-blox 5 GPS unit](http://www.seeedstudio.com/depot/grove-gps-p-959.html) (can you tell that Grove connectors were a big thing in this design?).  It utilizes the [I2C Device Library](https://github.com/jrowberg/i2cdevlib) to talk to the sensors.

This setup is big, really big, but it held up well in a 4" diameter high-power rocket.  I've since found it's missing something of critical importance: a [Gyro](https://www.sparkfun.com/pages/accel_gyro_guide).

-30-
