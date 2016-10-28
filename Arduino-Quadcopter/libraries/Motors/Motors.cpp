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
	startAll();
};

void Motors::startAll() {
	unsigned long begin = micros();
	while (micros() - begin <= escArmMicros) {
		PORTD |= B11110000;
		delayMicroseconds(escLowLimit - 100);
		PORTD &= B00001111;
		delayMicroseconds(1000);
	};
	motorsStarted = true;
};

void Motors::stopAll() {
	PORTD |= B11110000;
	delayMicroseconds(escLowLimit - 100);
	PORTD &= B00001111;
};

void Motors::staySilent() {
	if (motorsStarted) {
		stopAll();
	} else {
		outputAll(escLowLimit + 50, 0, 0, 0);
	}
};

void Motors::outputBackLeft(uint16_t duration) {
	PORTD |= B00010000;				//Turn on pin 4
	delayMicroseconds(duration);
	PORTD &= B11101111;				//Turn off pin 4
};

void Motors::outputFrontLeft(uint16_t duration) {
	PORTD |= B00100000;				//Turn on pin 5
	delayMicroseconds(duration);
	PORTD &= B11011111;				//Turn off pin 5
};

void Motors::outputBackRight(uint16_t duration) {
	PORTD |= B01000000;				//Turn on pin 6
	delayMicroseconds(duration);
	PORTD &= B10111111;				//Turn off pin 6
};

void Motors::outputFrontRight(uint16_t duration) {
	PORTD |= B10000000;				//Turn on pin 7
	delayMicroseconds(duration);
	PORTD &= B01111111;				//Turn off pin 7
};

void Motors::outputAll(uint16_t throttle, int16_t pitch, int16_t roll, int16_t yaw) {
	uint32_t currentTime = micros();
	PORTD |= B11110000;
	//Calculate the values for each motor
	uint32_t frontLeft 	= currentTime + throttle + pitch - roll + yaw;
	uint32_t frontRight = currentTime + throttle + pitch + roll - yaw;
	uint32_t backLeft 	= currentTime + throttle - pitch - roll - yaw;
	uint32_t backRight 	= currentTime + throttle - pitch + roll + yaw;
	while (PORTD > B00001000) {
		currentTime = micros();
		if (currentTime >= backLeft) {
			PORTD &= B11101111;
		};
		if (currentTime >= frontLeft) {
			PORTD &= B11011111;
		};
		if (currentTime >= backRight) {
			PORTD &= B10111111;
		};
		if (currentTime >= frontRight) {
			PORTD &= B01111111;
		};
	};
	delayMicroseconds(1);
};
