# Drone
The code for my quadcopter.
The main file the quadcopter is going to run on is testBareFinalDrone.ino
The files on which this file relies are either within the 'libraries' directory, or within the Arduino IDE as standard library.

This code is written for an Arduino Uno, with the ATmega328p microcontroller.
I'm going to buy the Arduino 101, so this code will be updated for this board

# IMU
My quadcopter uses the MPU-9050 IMU, whereof I only use only the accelerometer and the gyroscope. I couldn't get the compass working due to motors with a strong magnetic field. In the future I might add a feature that supports compasses. The calculation of pitch and yaw are written in the 'MoveMeter' library, which depends on the 'AK8975' (compass) and the 'MPU6050' (accel + gyro) libraries. Using a complementary filter and a modified version of the Direction Cosine Matrix I calculate the pitch and roll. It's a bit hard to read, because I wanted to keep things as fast as possible to get a loop time as low as possible.

# Electronic Speed Controller.
It communicates with the Afro 30A ESC's using PPM, in the library 'Motors'. It doesn't use digitalWrite(port), because this is a relative slow function, but instead manipulates the ports directly by setting bits high and low.

# Barometer
I'm not planning to implement a barometer in my quadcopter, because there are much variables unknown when using a barometer to calculate the height. The code to use the MS5611BA barometer is written in the library 'AltitudeMeter', which depends on the library 'MS561101BA'.

# Battery
This library calculates how many voltage is left in the battery, to calculate the value needed for voltage drop compensation.

# Calculator
In here is the P.I.D. algorithm I've written to keep the quadcopter in balance, and the algorithm to keep the quadcopter below 50m (dutch law). 
