#include "AltitudeMeter.h"

//PUBLIC
AltitudeMeter::AltitudeMeter(){
	pressure = temperature = groundPressure = groundTemperature = 0;
};

AltitudeMeter::~AltitudeMeter(){};

void AltitudeMeter::initialize() {
	// Suppose that the CSB pin is connected to GND.
  	// You'll have to check this on your breakout schematics
	barometer.init(MS561101BA_ADDR_CSB_LOW);
	delay(500);
}

void AltitudeMeter::setGroundPressure( double pressure) {
	groundPressure = pressure;
};

void AltitudeMeter::setGroundPressure() {
	double _pressure;
	while (pressure == 0 && (pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0) {
		delay(1);
	};
	for(int x = 0; x < 2000; x++) {
		while ((_pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0) {
			delay(1);
		};
		pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
		delay(1);
	};
	groundPressure = pressure;
}

void AltitudeMeter::setGroundTemperature(double temperature) {
	groundTemperature = temperature;
}

void AltitudeMeter::setGroundTemperature() {
	double _temperature;
	Serial.println("Going into loop!");
	while (temperature == 0 && (temperature = barometer.getTemperature(MS561101BA_OSR_256)) == 0) {
		delay(1);
	};
	Serial.println("Going into for-loop!");
	for(int x = 0; x < 2000; x++) {
		while ((_temperature = barometer.getTemperature(MS561101BA_OSR_256)) == 0) {
			delay(1);
		};
		temperature = temperature * temperatureGain + (1 - temperatureGain) * _temperature;
		delay(1);
	};
	groundTemperature = temperature;
}

void AltitudeMeter::setGroundParameters() {
	double _pressure = 0, _temperature = 0;
	Serial.println("Setting ground parameters");
	while (pressure == 0) {
		delay(15);
		pressure = barometer.getPressure(MS561101BA_OSR_256);
	};
	Serial.println("Out of pressure loop!");
	while (temperature == 0) {
		delay(15);
		temperature = barometer.getTemperature(MS561101BA_OSR_256);
	};
	Serial.print("Into the for-loop! Pressure: ");
	Serial.print(pressure);
	Serial.print(" Temperature ");
	Serial.println(temperature);
	for(int x = 0; x < 200; x++) {
		Serial.println(x);
		while ((_pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0 || (_temperature = barometer.getTemperature(MS561101BA_OSR_256)) == 0) {
			Serial.print("Pressure: ");
			Serial.print(_pressure);
			Serial.print(" Temperature: ");
			Serial.println(_temperature);
			delay(15);
		};
		temperature = temperature * temperatureGain + (1 - temperatureGain) * _temperature;
		pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
		delay(1);
	};
	groundPressure = pressure;
	groundTemperature = temperature;
};

double AltitudeMeter::getHeight() {
	double _pressure;
	while ((_pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0) {
		delay(10);
	};
	pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
	return heightConstant * groundTemperature * log(pressure / groundPressure);
}

double AltitudeMeter::getOldHeight() {
	//temperature = temperature * temperatureGain + (1 - temperatureGain) * barometer.getTemperature(MS561101BA_OSR_256);
	double _pressure;
	while ((_pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0) {
		delayMicroseconds(100);
	};
	pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
	return (pow(pressure / groundPressure, 1 / 5.25588) - 1.0) / -0.0000225577;
	//return ((pow(groundPressure / pressure, 1/5.25588) - 1.0) * (temperature + 273.15)) / 0.0065;
};

double AltitudeMeter::getGroundPressure() {
	return groundPressure;
};

double AltitudeMeter::getGroundTemperature() {
	return groundTemperature;
}

double AltitudeMeter::getPressure() {
	double _pressure;
	while ((_pressure = barometer.getPressure(MS561101BA_OSR_256)) == 0) {
		delayMicroseconds(100);
	};
	pressure = pressure * pressureGain + (1 - pressureGain) * _pressure;
	return pressure;
};

double AltitudeMeter::getTemperature() {
	double _temperature;
	while ((_temperature = barometer.getTemperature(MS561101BA_OSR_256)) == 0) {
		delayMicroseconds(100);
	}
	temperature = temperature * temperatureGain + (1 - temperatureGain) * _temperature;
	return temperature;
};
