#include <Arduino.h>
#include "config.h"

class Calculator {
public:
	Calculator();
	~Calculator();
	void PIDPitchRoll(volatile uint16_t signals[], double *mpuPitch, double *mpuRoll, double *outputPitch, double *outputRoll);
	int PIDHeight(double *height);
	void resetHeightParameters();
	void resetPitchRollParameters();
	void resetAll();
	
	
private:
	double lastRollError, lastPitchError;
	double rollErrorSum, pitchErrorSum;
	double lastHeightError;
	double heightErrorSum;
};
