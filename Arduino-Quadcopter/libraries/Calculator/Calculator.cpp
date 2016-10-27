#include "Calculator.h"

Calculator::Calculator() {};

Calculator::~Calculator() {};

void Calculator::PIDPitchRoll(volatile uint16_t signals[], double *mpuPitch, double *mpuRoll, double *outputPitch, double *outputRoll) {
	if (signals[rollChannel] > 1460 && signals[rollChannel] < 1500) {
		signals[rollChannel] = 1478; // (radioLowLimit + radioHighLimit) / 2
	}
	if (signals[pitchChannel] > 1460 && signals[pitchChannel] < 1500) {
		signals[pitchChannel] = 1478;
	}
	if (signals[yawChannel] > 1460 && signals[yawChannel] < 1500) {
		signals[yawChannel] = 1478;
	}
	double remoteRoll = ((signals[rollChannel] - radioLowLimit)  * 90.0 / (radioHighLimit - radioLowLimit)) - 45;
	double remotePitch = ((signals[pitchChannel] - radioLowLimit) * 90.0 / (radioHighLimit - radioLowLimit)) - 45;
	
	if (remoteRoll > rollMaxAngle) {
		remoteRoll = rollMaxAngle;
	} else if (remoteRoll < rollMaxAngle * -1) {
		remoteRoll = rollMaxAngle * -1;
	}
	if (remotePitch > pitchMaxAngle) {
		remotePitch = pitchMaxAngle;
	} else if (remotePitch < pitchMaxAngle * -1) {
		remotePitch = pitchMaxAngle * -1;
	}
	
	double currentError = (*mpuPitch - remotePitch);
	pitchErrorSum += currentError;
	if (pitchErrorSum > pitchMaxValue) {
		pitchErrorSum = pitchMaxValue;
	} else if (pitchErrorSum < pitchMaxValue * -1) {
		pitchErrorSum = pitchMaxValue * -1;
	};
	*outputPitch = pitchPGain * currentError + pitchIGain * timeConstant * pitchErrorSum + pitchDGain * (currentError - lastPitchError) / timeConstant;
	if (*outputPitch > pitchMaxValue) {
		*outputPitch = pitchMaxValue;
	} else if (*outputPitch < pitchMaxValue * -1) {
		*outputPitch = pitchMaxValue * -1;
	};
	lastPitchError = currentError;
	
	currentError = *mpuRoll - remoteRoll;
	rollErrorSum += currentError;
	if (rollErrorSum > rollMaxValue) {
		rollErrorSum = rollMaxValue;
	} else if (rollErrorSum < rollMaxValue * -1) {
		rollErrorSum = rollMaxValue * -1;
	};
	*outputRoll = rollPGain * currentError + pitchIGain * timeConstant * rollErrorSum + rollDGain * (currentError - lastRollError) / timeConstant;
	if (*outputRoll > rollMaxValue) {
		*outputRoll = rollMaxValue;
	} else if (*outputRoll < rollMaxValue * -1) {
		*outputRoll = rollMaxValue * -1;
	};
	lastRollError = currentError;
};

int Calculator::PIDHeight(double *height) {
	int currentError = *height - maxHeight;
	heightErrorSum += currentError * heightIGain;
	int result = (int)(heightPGain * currentError + heightErrorSum * timeConstant + heightDGain * (currentError - lastHeightError) / timeConstant);
	lastHeightError = currentError;
	return result;
};

void Calculator::resetHeightParameters() {
	heightErrorSum = lastHeightError = 0;
};

void Calculator::resetAll() {
	lastRollError = lastPitchError = lastHeightError = 0;
	rollErrorSum = pitchErrorSum = heightErrorSum = 0;
};

void Calculator::resetPitchRollParameters() {
	lastRollError = lastPitchError = 0;
	rollErrorSum = pitchErrorSum = 0;
};
