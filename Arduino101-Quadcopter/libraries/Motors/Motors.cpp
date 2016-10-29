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
		setPort4High();
		setPort5High();
		setPort6High();
		setPort7High();
		delayMicroseconds(escLowLimit - 100);
		setPort4Low();
		setPort5Low();
		setPort6Low();
		setPort7Low();
		delayMicroseconds(1000);
	};
	motorsStarted = true;
};

void Motors::stopAll() {
	setPort4High();
	setPort5High();
	setPort6High();
	setPort7High();
	delayMicroseconds(escLowLimit - 100);
	setPort4Low();
	setPort5Low();
	setPort6Low();
	setPort7Low();
};

void Motors::staySilent() {
	if (motorsStarted) {
		stopAll();
	} else {
		outputAll(escLowLimit + 100, escLowLimit + 100, escLowLimit + 100, escLowLimit + 100);
	}
};

void Motors::outputBackLeft(uint16_t duration) {
	setPort4High();				//Turn on pin 4
	delayMicroseconds(duration);
	setPort4Low();				//Turn off pin 4
};

void Motors::outputFrontLeft(uint16_t duration) {
	setPort5High();				//Turn on pin 5
	delayMicroseconds(duration);
	setPort5Low();				//Turn off pin 5
};

void Motors::outputBackRight(uint16_t duration) {
	setPort6High();				//Turn on pin 6
	delayMicroseconds(duration);
	setPort6Low();				//Turn off pin 6
};

void Motors::outputFrontRight(uint16_t duration) {
	setPort7High();				//Turn on pin 7
	delayMicroseconds(duration);
	setPort7Low();				//Turn off pin 7
};

void Motors::outputAll(uint16_t throttle, int16_t pitch, int16_t roll, int16_t yaw) {
	uint8_t on = B00001111;
	setPort4High();
	setPort5High();
	setPort6High();
	setPort7High();
	uint32_t currentTime = micros();
	uint32_t frontLeft 	= currentTime + throttle - pitch + roll + yaw;
	uint32_t frontRight = currentTime + throttle - pitch - roll - yaw;
	uint32_t backLeft 	= currentTime + throttle + pitch + roll - yaw;
	uint32_t backRight 	= currentTime + throttle + pitch - roll + yaw;
	while (on > B00000001) {
		currentTime = micros();
		if (currentTime >= backLeft) {
			setPort4Low();
			on &= B00001110;
		};
		if (currentTime >= frontLeft) {
			setPort5Low();
			on &= B00001101;
		};
		if (currentTime >= backRight) {
			setPort6Low();
			on &= B00001011;
		};
		if (currentTime >= frontRight) {
			setPort7Low();
			on &= B00000111;
		};
	};
};
