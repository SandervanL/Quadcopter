#include <Arduino.h>
static const int batteryLowValue = 1020;
static const float batteryCompCoeff = 0.92;


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