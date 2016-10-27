#include <CurieIMU.h>
#include "config.h"

class MoveMeter {
	public:
		MoveMeter();
		~MoveMeter();
		void initialize();
		bool testConnection();
		double getTemperature();
		double getPitch();
		double getRoll();
		void getPitchRoll(double &pitch, double &roll);
	
	private:
		double _pitch, _roll;
		int16_t ax, ay, az, gx, gy, gz;
		void ComplementaryPitch(double dt);
		void ComplementaryRoll (double dt);
};
