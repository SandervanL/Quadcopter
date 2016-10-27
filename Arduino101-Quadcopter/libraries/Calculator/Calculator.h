#include <Arduino.h>
#include "config.h"

class Calculator {
public:
	Calculator();
	~Calculator();
	void PIDPitchRoll(volatile uint32_t signals[], double mpuPitch, double mpuRoll, int16_t &outputPitch, int16_t &outputRoll);
	int PIDHeight(double height);
	void resetHeightParameters();
	void resetPitchRollParameters();
	void resetAll();
	
	
private:
	uint32_t lastHeightTimer;
	double lastRollError, lastPitchError, lastHeightError;
	double rollErrorSum, pitchErrorSum, heightErrorSum;
};
