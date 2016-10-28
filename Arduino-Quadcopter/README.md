# Drone
The code for my quadcopter.
The main file the quadcopter is going to run on is testBareFinalDrone.ino
The files on which this file relies are either within the 'libraries' directory, or within the Arduino IDE as standard library.

This code is written for an Arduino Uno, with the ATmega328p microcontroller.

# IMU
My quadcopter uses the MPU-9050 IMU, whereof I only use only the accelerometer and the gyroscope. I couldn't get the compass working due to the motors generating a strong magnetic field. In the future I might add a feature that supports compasses. The calculation of pitch and roll are written in the 'MoveMeter' library, which depends on the 'MPU6050' (accel + gyro) library. Using a complementary filter and a modified version of the Direction Cosine Matrix I calculate the pitch and roll. It's a bit hard to read, because I wanted to keep things as fast as possible to get a cycle time as low as possible.

# Electronic Speed Controller.
The Arduino communicates with the Afro 30A ESC's using PPM, in the library 'Motors'. It doesn't use digitalWrite(port), because this is a relative slow function, but instead manipulates the ports directly by setting bits high and low in the PORTD registers.

# Barometer
I'm not planning on implementing a barometer in my quadcopter, because there are a lot of variables unknown when using a barometer to calculate the height. The code to use the MS5611BA barometer is written in the library 'AltitudeMeter', which depends on the library 'MS561101BA'. For my barometer I've tried to use the MS651101 chip.

# Battery
This library calculates how many voltage is left in the battery, to calculate the compensation needed for voltage drop.

# Calculator
In here is the P.I.D. algorithm I've written to keep the quadcopter in balance, and the algorithm to keep the quadcopter below 50m (dutch law). In my code, I do not use the height P.I.D. algorithm as my first aim is to get the copter flying