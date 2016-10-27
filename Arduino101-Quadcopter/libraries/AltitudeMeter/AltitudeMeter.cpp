#include "AltitudeMeter.h"

//PUBLIC
AltitudeMeter::AltitudeMeter(){;};

AltitudeMeter::~AltitudeMeter(){};

void AltitudeMeter::initialize() {
	// Suppose that the CSB pin is connected to GND.
  	// You'll have to check this on your breakout schematics
  	barometer = MS561101BA();
	barometer.init(MS561101BA_ADDR_CSB_LOW);
	delay(250);
	pressure = temperature = groundPressure = 0;
}

void AltitudeMeter::setGroundPressure( double pressure) {
	groundPressure = pressure;
};

void AltitudeMeter::setGroundPressure() {
	double _pressure, _temperature;
	for(int x = 0; x < 2000; x++) {
		while ((_pressure = barometer.getPressure(MS561101BA_OSR_4096)) == 0 || (_temperature = barometer.getTemperature(MS561101BA_OSR_4096)) == 0) {
			delayMicroseconds(1000);
		};
		temperature = temperature * temperatureGain + (1 - temperatureGain) * _temperature;
		pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
		delayMicroseconds(1000);
	};
	groundPressure = pressure;
};

double AltitudeMeter::getHeight() {
	//temperature = temperature * temperatureGain + (1 - temperatureGain) * barometer.getTemperature(MS561101BA_OSR_4096);
	double _pressure;
	while ((_pressure = barometer.getPressure(MS561101BA_OSR_4096)) == 0) {
		delayMicroseconds(100);
	};
	pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
	return (pow(pressure / groundPressure, 1 / 5.25588) - 1.0) / -0.0000225577;
	//return ((pow(groundPressure / pressure, 1/5.25588) - 1.0) * (temperature + 273.15)) / 0.0065;
};

double AltitudeMeter::getGroundPressure() {
	return groundPressure;
};

double AltitudeMeter::getPressure() {
	double _pressure;
	while ((_pressure = barometer.getPressure(MS561101BA_OSR_4096)) == 0) {
		delayMicroseconds(100);
	};
	pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
	return pressure;
};

double AltitudeMeter::getTemperature() {
	double _temperature;
	while ((_temperature = barometer.getTemperature(MS561101BA_OSR_4096)) == 0) {
		delayMicroseconds(100);
	}
	temperature = temperature * temperatureGain + (1 - temperatureGain) * _temperature;
	return temperature;
};
