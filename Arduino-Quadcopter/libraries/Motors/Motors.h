#include <Arduino.h>
#include "config.h"

/*
FRONT_LEFT 3
FRONT_RIGHT 4
BACK_LEFT 5
BACK_RIGHT 6
*/

class Motors {
public:
	Motors();
	~Motors();
	void initialize();
	void startAll();
	void stopAll();
	void staySilent();
	void outputFrontLeft	(uint16_t duration);
	void outputFrontRight	(uint16_t duration);
	void outputBackLeft		(uint16_t duration);
	void outputBackRight	(uint16_t duration);
	void outputAll			(uint16_t throttle, int16_t pitch, int16_t roll, int16_t yaw);
	bool motorsStarted;
};
