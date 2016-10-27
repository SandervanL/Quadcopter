#include "Battery.h"

Battery::Battery() {
	_port = 0;
}

Battery::~Battery(){};

void Battery::setPort(int port) {
	_port = port;
}

void Battery::initialize() {
	for (int i = 0; i < 1000; i++) {
		refreshVoltage();
		delay(1);
	}
}

bool Battery::isLow() {
	unsigned int value = analogRead(_port);
	if (value > 600 && value < batteryLowValue) {
		return true;
	}
	return false;
}

int Battery::getRawValue() {
	return analogRead(_port);
}

float Battery::getVoltage() {
	refreshVoltage();
	return batteryVoltage * 12.6 / 1023.0;
}

float Battery::getCorrection() {
	refreshVoltage();
	return (1240 - batteryVoltage) / 3500.0 + 1;
}


void Battery::refreshVoltage() {
	batteryVoltage = batteryVoltage * batteryCompCoeff + (1 - batteryCompCoeff) * analogRead(_port);
}
