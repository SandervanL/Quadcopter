#include <Arduino.h>
#include "config.h"


class Battery {
	public:
	Battery();
	~Battery();
	void setPort(int port);
	void initialize();
	bool isLow();
	int getRawValue();
	float getVoltage();
	float getCorrection();
	
	private:
		void refreshVoltage();
		float batteryVoltage;
		int _port;
};