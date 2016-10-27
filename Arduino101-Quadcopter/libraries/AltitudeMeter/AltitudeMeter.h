#include <Wire.h>
#include <MS561101BA.h>
static const double pressureGain = 0.6;
static const double temperatureGain = 0.6;

class AltitudeMeter {
	public:
		AltitudeMeter();
		~AltitudeMeter();
		void  initialize();
		void  setGroundPressure();
		void  setGroundPressure(double pressure);
		double getHeight();
		double getGroundPressure();
		double getPressure();
		double getTemperature();
	
	private:
		double groundPressure, pressure;
		double temperature;
		MS561101BA barometer;
};

/*The dutch law states that private and commercial minidrones aren't allowed to fly higher than 50 meters
    //In this one, there is nothing going on
    if (height < 45 && throttle50m != 0) {
      hoverThrottle = throttle50m;
      throttle50m = 0;
      lastHeight = 0;
      //Here the height is over 50 meters for the first time, and we set throttle50m to hovering speed.
    } else if (height >= 50 && lastHeight == 0) {
      lastHeight = height;
      throttle50m = hoverThrottle;
      //Here we haven't gone lower since the last measurement, so we lower the throttle50m value and remember the current height
    } else if (height >= 50 && height > lastHeight && lastHeight != 0) {
      lastHeight = height;
      throttle50m -= 1;
    };
	
	//If more throttle is wanted than we allow, we set the throttle to the throttle we allow
    if (throttle50m != 0 && throttle > throttle50m) {
      throttle = throttle50m;
    };
   */
