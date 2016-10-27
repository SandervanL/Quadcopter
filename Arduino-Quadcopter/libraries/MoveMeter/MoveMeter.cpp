#include "MoveMeter.h"

//PUBLIC
MoveMeter::MoveMeter() {
	accelGyro = MPU6050();
	//compass = AK8975();
	_pitch = _roll = 0;
};

MoveMeter::~MoveMeter() {};

void MoveMeter::initialize() {
	accelGyro.initialize();
	accelGyro.setFIFOEnabled(false);
	//compass.initialize();
	delay(100);
	//Earlier defined values by MPU6050_calibration or calculateOffsets
	accelGyro.setXAccelOffset(accelXOffset);
	accelGyro.setYAccelOffset(accelYOffset);
	accelGyro.setZAccelOffset(accelZOffset);
	accelGyro.setXGyroOffset(gyroXOffset);
	accelGyro.setYGyroOffset(gyroYOffset);
	accelGyro.setZGyroOffset(gyroZOffset);
};

bool MoveMeter::testMPUConnection() {
	return accelGyro.testConnection();
};

/*bool MoveMeter::testCompassConnection() {
	return compass.testConnection();
};*/

double MoveMeter::getTemperature() {
	return (double) (accelGyro.getTemperature() + 12412.0) / 340.0;
};

double MoveMeter::getPitch() {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	unsigned long currentTime = micros();
	ComplementaryPitch((currentTime - pitchTimer) / 1000000.0f);
	pitchTimer = currentTime;
	return _pitch * RAD_TO_DEG;
};

double MoveMeter::getRoll() {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	unsigned long currentTime = micros();
	ComplementaryRoll ((currentTime - rollTimer) / 1000000.0f);
	rollTimer = currentTime;
	return _roll * RAD_TO_DEG;
};

/*double MoveMeter::getYaw() {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	compass.getHeading(&mx, &my, &mz);
	unsigned long currentTime = micros();
	ComplementaryPitch((currentTime - pitchTimer) / 1000000.0f);
	ComplementaryRoll ((currentTime - rollTimer) / 1000000.0f);
	rollTimer = pitchTimer = yawTimer = currentTime;
	double mNorm = sqrt((long)mx * mx + (long)my * my + (long) mz * mz);
	double My = my / mNorm;
	double Mz = mz / mNorm;
	double sinPitch = sin(_pitch), sinRoll = sin(_roll), cosRoll = sqrt(1 - sinRoll * sinRoll);
	return atan2((-My*cosRoll + Mz*sinRoll), ((mx / mNorm) *sqrt(1 - sinPitch * sinPitch) + My*sinPitch*sinRoll+ Mz*sinPitch*cosRoll)) * RAD_TO_DEG;
};*/


//If you use this function, only use this function
void MoveMeter::getPitchRoll(double *pitch, double *roll) {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
#ifdef DEBUG
	Serial.print(ax);	Serial.print(",");
	Serial.print(ay);	Serial.print(",");
	Serial.print(az);	Serial.print(",");
	Serial.print(gx);	Serial.print(",");
	Serial.print(gy);	Serial.print(",");
	Serial.print(gz);	Serial.print(",");
//	Serial.print(timeConstant);	Serial.print(",");
#endif
	_pitch = compCoeff*(_pitch + (gy * timeConstant / gyroSensitivity)) + (1.0f - compCoeff) * atan2(ax, sqrt((long)ay*ay + (long) az*az)) * 3.0 * RAD_TO_DEG;
	_roll =  compCoeff*(_roll  +(-gx * timeConstant / gyroSensitivity)) + (1.0f - compCoeff) * atan2(ay, sqrt((long)ax*ax + (long) az*az)) * 3.0 * RAD_TO_DEG;
	*pitch =  _pitch;
	*roll =  _roll;
};




/*
										y * cos(roll) + z * sin(roll)
				___________________________________________________________________________
	tan(yaw) = 		x * cos(pitch) + y * sin(pitch) * sin(roll) + z * sin(pitch) * cos(roll)
	
*/
/*void MoveMeter::getPitchRollYaw(double *pitch, double *roll, double *yaw) {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	compass.getHeading(&mx, &my, &mz);
	unsigned long currentTime = micros();
	ComplementaryPitch((currentTime - pitchTimer) / 1000000.0f);
	ComplementaryRoll ((currentTime - rollTimer) / 1000000.0f);
	rollTimer = pitchTimer = yawTimer = currentTime;
	double mNorm = sqrt(mx * mx + my * my + mz * mz);
	double My = my / mNorm;
	double Mz = mz / mNorm;
	double sinPitch = sin(_pitch), sinRoll = sin(_roll), cosRoll = sqrt(1 - sinRoll * sinRoll);
	*pitch = _pitch;
	*roll = _roll;
	//https://sites.google.com/site/myimuestimationexperience/sensors/magnetometer
	*yaw = atan2((-My*cosRoll + Mz*sinRoll), ((mx / mNorm) *sqrt(1 - sinPitch * sinPitch) + My*sinPitch*sinRoll+ Mz*sinPitch*cosRoll));
};*/


/*Function which takes as parameter an array of at least size 3, and fills this array with in order pitch, roll, yaw
void MoveMeter::getPitchRollYaw(double *pry) {
	accelGyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	compass.getHeading(&mx, &my, &mz);
	unsigned long currentTime = micros();
	ComplementaryPitch((currentTime - pitchTimer) / 1000000.0f);
	ComplementaryRoll ((currentTime - rollTimer) / 1000000.0f);
	rollTimer = pitchTimer = yawTimer = currentTime;
	double mNorm = sqrt(mx * mx + my * my + mz * mz);
	double My = my / mNorm;
	double Mz = mz / mNorm;
	double sinPitch = sin(_pitch), sinRoll = sin(_roll), cosRoll = sqrt(1 - sinRoll * sinRoll);
	pry[0] = _pitch;
	pry[1] = _roll;
	pry[2] = atan2((-My*cosRoll + Mz*sinRoll), ((mx / mNorm) *sqrt(1 - sinPitch * sinPitch) + My*sinPitch*sinRoll+ Mz*sinPitch*cosRoll));
};*/



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
	_pitch = compCoeff*(_pitch + (gy * dt / gyroSensitivity)) + (1.0f - compCoeff) *atan2(ax, sqrt((long)ay*ay + (long) az*az));
};

void MoveMeter::ComplementaryRoll(double dt) {
	_roll = compCoeff*(_roll + (-gx * dt / gyroSensitivity)) + (1.0f - compCoeff) *atan2(ay, sqrt((long) ax*ax + (long) az*az));
}
