#include "Motors.h"
 /*
BACK_LEFT 4
FRONT_LEFT 5
BACK_RIGHT 6
FRONT_RIGHT 7
*/
Motors::Motors(){
	motorsStarted = false;
};
Motors::~Motors(){};

void Motors::initialize() {
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
	pinMode(7, OUTPUT);
};

void Motors::startAll() {
	uint32_t begin = micros();
	while (micros() - begin <= escArmMicros) {
		digitalWrite(4, HIGH);
		digitalWrite(5, HIGH);
		digitalWrite(6, HIGH);
		digitalWrite(7, HIGH);
		delayMicroseconds(escLowLimit - 100);
		digitalWrite(4, LOW);
		digitalWrite(5, LOW);
		digitalWrite(6, LOW);
		digitalWrite(7, LOW);
		delayMicroseconds(1000);
	};
	motorsStarted = true;
};

void Motors::stopAll() {
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
	digitalWrite(6, HIGH);
	digitalWrite(7, HIGH);
	delayMicroseconds(escLowLimit - 100);
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	digitalWrite(6, LOW);
	digitalWrite(7, LOW);
};

void Motors::staySilent() {
	if (motorsStarted) {
		stopAll();
	} else {
		outputAll(escLowLimit + 100, escLowLimit + 100, escLowLimit + 100, escLowLimit + 100);
	}
};

void Motors::outputBackLeft(uint16_t duration) {
	digitalWrite(4, HIGH);				//Turn on pin 4
	delayMicroseconds(duration);
	digitalWrite(4, LOW);				//Turn off pin 4
};

void Motors::outputFrontLeft(uint16_t duration) {
	digitalWrite(5, HIGH);				//Turn on pin 5
	delayMicroseconds(duration);
	digitalWrite(5, LOW);				//Turn off pin 5
};

void Motors::outputBackRight(uint16_t duration) {
	digitalWrite(6, HIGH);				//Turn on pin 6
	delayMicroseconds(duration);
	digitalWrite(6, LOW);				//Turn off pin 6
};

void Motors::outputFrontRight(uint16_t duration) {
	digitalWrite(7, HIGH);				//Turn on pin 7
	delayMicroseconds(duration);
	digitalWrite(7, LOW);				//Turn off pin 7
};

void Motors::outputAll(uint16_t throttle, int16_t pitch, int16_t roll, int16_t yaw) {
	byte on = B00001111;
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
	digitalWrite(6, HIGH);
	digitalWrite(7, HIGH);
	uint32_t currentTime = micros();
	uint32_t frontLeft 	= currentTime + throttle - pitch + roll + yaw;
	uint32_t frontRight = currentTime + throttle - pitch - roll - yaw;
	uint32_t backLeft 	= currentTime + throttle + pitch + roll - yaw;
	uint32_t backRight 	= currentTime + throttle + pitch - roll + yaw;
	while (on > B00000001) {
		currentTime = micros();
		if (currentTime >= backLeft) {
			digitalWrite(4, LOW);
			on &= B00001110;
		};
		if (currentTime >= frontLeft) {
			digitalWrite(5, LOW);
			on &= B00001101;
		};
		if (currentTime >= backRight) {
			digitalWrite(6, LOW);
			on &= B00001011;
		};
		if (currentTime >= frontRight) {
			digitalWrite(7, LOW);
			on &= B00000111;
		};
	};
};
