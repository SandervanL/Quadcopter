#include "MoveMeter.h"

//PUBLIC
MoveMeter::MoveMeter() {
	CurieIMU.initialize();
	_pitch = _roll = 0;
};

MoveMeter::~MoveMeter() {};

void MoveMeter::initialize() {
	CurieIMU.setGyroFIFOEnabled(false);
	CurieIMU.setAccelFIFOEnabled(false);
	//compass.initialize();
	delay(100);
	//Earlier defined values by MPU6050_calibration or calculateOffsets
	/*CurieIMU.setXAccelOffset(accelXOffset);
	CurieIMU.setYAccelOffset(accelYOffset);
	CurieIMU.setZAccelOffset(accelZOffset);
	CurieIMU.setXGyroOffset(gyroXOffset);
	CurieIMU.setYGyroOffset(gyroYOffset);
	CurieIMU.setZGyroOffset(gyroZOffset);*/
};

bool MoveMeter::testConnection() {
	return CurieIMU.testConnection();
};

double MoveMeter::getTemperature() {
	return (double) (CurieIMU.getTemperature() + 12412.0) / 340.0;
};

double MoveMeter::getPitch() {
	CurieIMU.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	ComplementaryPitch(timeConstant);
	return _pitch * RAD_TO_DEG;
};

double MoveMeter::getRoll() {
	CurieIMU.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	ComplementaryRoll (timeConstant);
	return _roll * RAD_TO_DEG;
};

void MoveMeter::getPitchRoll(double &pitch, double &roll) {
	CurieIMU.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
#ifdef DEBUG
	Serial.print(ax);	Serial.print(",");
	Serial.print(ay);	Serial.print(",");
	Serial.print(az);	Serial.print(",");
	Serial.print(gx);	Serial.print(",");
	Serial.print(gy);	Serial.print(",");
	Serial.print(gz);	Serial.print(",");
#endif
	ComplementaryPitch(timeConstant);
	ComplementaryRoll(timeConstant);
	pitch = _pitch * RAD_TO_DEG;
	roll = _roll * RAD_TO_DEG;
};




/* OBSOLETE COMMENT
										y * cos(roll) + z * sin(roll)
				___________________________________________________________________________
	tan(yaw) = 		x * cos(pitch) + y * sin(pitch) * sin(roll) + z * sin(pitch) * cos(roll)
	
*/

//PRIVATE

/*
	DEFENITION FOR ACCELERATION ANGLE:
						ax
		       	______________
	tan(x) =        __________
         		\  /  2		2
	        	 \/	ay	+ az

DEFENITION FOR FINAL ANGLE:

	Final Angle = 0.98 * (angle + (-gyro_data * dt)) + (1- 0.98) * acc_angle
	
	Where:
		angle = The previous angle (calculated at the previous cycle).
		dt = delta T: The difference in time in seconds.
		0.98 = A constant (compCoeff).
		acc_angle = The angle calculated with the accelerometer calculation above.
*/
	
void MoveMeter::ComplementaryPitch(double dt) {
	_pitch = compCoeff*(_pitch + (-gy * dt / gyroSensitivity)) + (1.0f - compCoeff)*atan2(ax, sqrt((long)ay*ay + (long) az*az));
};

void MoveMeter::ComplementaryRoll(double dt) {
	_roll = compCoeff*(_roll + (-gx * dt / gyroSensitivity)) + (1.0f - compCoeff)*atan2(ay, sqrt((long) ax*ax + (long) az*az));
}
