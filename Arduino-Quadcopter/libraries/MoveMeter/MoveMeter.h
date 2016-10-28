#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <AK8975.h>
#include "config.h"

class MoveMeter {
	public:
		MoveMeter();
		~MoveMeter();
		void initialize();
		bool testMPUConnection();
//		bool testCompassConnection();
		//No mpu calibration anymore: this is done during initializing after the connection is known
		double getTemperature();
		double getPitch();
		double getRoll();
//		double getYaw();
		//If you use this function, only use this function
		void getPitchRoll(double &pitch, double &roll);
//		void getPitchRollYaw(double *pitch, double *roll, double *yaw);
//		void getPitchRollYaw(double *pry);
	
	private:
		MPU6050 accelGyro;
//		AK8975 compass;
		double _pitch, _roll;
		int16_t ax, ay, az, gx, gy, gz;//, mx, my, mz;
		uint32_t pitchTimer, rollTimer; /*yawTimer,*/
		void ComplementaryPitch(double dt);
		void ComplementaryRoll (double dt);
		bool firstAngle;
};
