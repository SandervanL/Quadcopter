#include "Arduino.h"

//Radio control variables
volatile uint32_t signals[8];     //Array which contains the signals of the remote control in microseconds (updated by ISR(PCINT_vect))
volatile uint32_t signalBegin[8]; //Array with beginvalues of the remote-control signals

void A0Interrupt() {
	if (!signalBegin[0]) {
		signalBegin[0] = micros();
	} else {
		signals[0] = micros() - signalBegin[0];
		signalBegin[0] = 0;
	}
}

void A1Interrupt() {
	if (!signalBegin[1]) {
		signalBegin[1] = micros();
	} else {
		signals[1] = micros() - signalBegin[1];
		signalBegin[1] = 0;
	}
}

void A2Interrupt() {
	if (!signalBegin[2]) {
		signalBegin[2] = micros();
	} else {
		signals[2] = micros() - signalBegin[2];
		signalBegin[2] = 0;
	}
}

void A3Interrupt() {
	if (!signalBegin[3]) {
		signalBegin[3] = micros();
	} else {
		signals[3] = micros() - signalBegin[3];
		signalBegin[3] = 0;
	}
}

void A4Interrupt() {
	if (!signalBegin[4]) {
		signalBegin[4] = micros();
	} else {
		signals[4] = micros() - signalBegin[4];
		signalBegin[4] = 0;
	}
}

void A5Interrupt() {
	if (!signalBegin[5]) {
		signalBegin[5] = micros();
	} else {
		signals[5] = micros() - signalBegin[5];
		signalBegin[5] = 0;
	}
}

void initializeRemoteControl() {
	//Enable the pull-up resistor for better values
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	pinMode(A4, INPUT_PULLUP);
	pinMode(A5, INPUT_PULLUP);
	
	//Setup the interrupts to watch the remote-control values change
	attachInterrupt(A0, A0Interrupt, CHANGE);
	attachInterrupt(A1, A1Interrupt, CHANGE);
	attachInterrupt(A2, A2Interrupt, CHANGE);
	attachInterrupt(A3, A3Interrupt, CHANGE);
	attachInterrupt(A4, A4Interrupt, CHANGE);
	attachInterrupt(A5, A5Interrupt, CHANGE);
}