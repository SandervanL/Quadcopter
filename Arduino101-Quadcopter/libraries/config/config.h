#ifndef CONFIG_DRONE_H
	#define CONFIG_DRONE_H
//#define DEBUG
//General constants
static const double timeConstant = 0.0036; //Time constant between loops: 3500 microseconds
static const uint8_t warningLED = 13;//The port where the warning LED has to be connected to
static const uint8_t timePort = 12;//The port which switches high and low to be able to measure the refresh rate with another arduino

//MoveMeter constants
static const double compCoeff = 0.9f;
static const double gyroSensitivity = 50.0f;

static const int16_t accelXOffset = -2886;//-2904;
static const int16_t accelYOffset = -341;//-339;
static const int16_t accelZOffset = 4803;//4804;
static const int16_t gyroXOffset = 60;//76;
static const int16_t gyroYOffset = 7;//4;
static const int16_t gyroZOffset = 23;//34;

//Calculator constants
static const double pitchPGain = 2.0;
static const double pitchIGain = 0;
static const double pitchDGain = 0.0;

static const double rollPGain = 2.0;
static const double rollIGain = 0;
static const double rollDGain = 0.0;

static const double yawPGain = 0;
static const double yawIGain = 0;
static const double yawDGain = 0;

static const uint8_t yawChannel = 0;
static const uint8_t throttleChannel = 1;
static const uint8_t pitchChannel = 2;
static const uint8_t rollChannel = 3;

static const uint16_t throttleLowLimit = 1052;
static const uint16_t throttleHighLimit = 1900;

static const uint16_t radioLowLimit = 968;
static const uint16_t radioHighLimit = 1988;

static const uint8_t rollMaxAngle = 45;
static const uint8_t pitchMaxAngle = 45;

static const int16_t rollMaxValue = 400;
static const int16_t pitchMaxValue = 400;
static const uint8_t yawMaxValue = 50;

static const int8_t maxHeight = 50;
static const double heightPGain = 0;
static const double heightIGain = 0;
static const double heightDGain = 0;

//AltitudeMeter constants
static const double pressureGain = 0.6;
static const double temperatureGain = 0.6;
static const double heightConstant = 8.31432 / (9.819296623 * 0.0289644);

//Battery constants
static const int batteryLowValue = 1020;
static const float batteryCompCoeff = 0.92;

//Motors constants
static const uint16_t escLowLimit = 1060;
static const uint16_t escHighLimit = 1860;
static const uint16_t escArmMicros = 40000;


#endif