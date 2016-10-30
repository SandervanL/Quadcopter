#define _USE_MATH_DEFINES
#include <cstdio>
#include "SerialClass.h"
#include <GL\glew.h>
#include <GL\glut.h>
#include <math.h>
#include <cmath>
#include <sstream>
#include "C:\\Users\\Sander\\Documents\\Code\\Drone\\Arduino-Quadcopter\\libraries\\config\\config.h"

static const double radToDeg = 180.0 / M_PI;

/*
*	Boundaries: x: -2.8 - 2.8
				y: -5	- 5
*/

Serial comPort;
char portName[6] = "";
uint32_t baudRate;

int16_t ax, ay, az, gx, gy, gz;
double arduinoPitch, arduinoRoll;
uint16_t throttleChannelValue, pitchChannelValue, rollChannelValue, yawChannelValue;
uint16_t throttle;
int16_t pitch, roll, yaw;
uint16_t frontLeft, frontRight, backLeft, backRight;

double value;
double accelPitch, gyroPitch, compPitch;
double accelRoll, gyroRoll, compRoll;

double previousAccelPitch, previousGyroPitch, previousCompPitch;
double previousAccelRoll, previousGyroRoll, previousCompRoll;
double previousArduinoPitch, previousArduinoRoll;
double previousThrottleChannel, previousPitchChannel, previousRollChannel, previousYawChannel;
double previousThrottle, previousPitch, previousRoll, previousYaw;
double previousFrontLeft, previousFrontRight, previousBackLeft, previousBackRight;

int pitchWindow, rollWindow, remoteWindow, angleWindow, motorsWindow;
double previousXPos = -5, xPos = -5;

void window();
void onReshape(int width, int height);
void getPortName();
int getBaudRate();
int getValues();
void drawPitch();
void drawRoll();
void drawRemote();
void drawAngles();
void drawMotors();
void printError(DWORD error);
double map(double x, double in_min, double in_max, double out_min, double out_max);


int main(int argc, char *argv[]) {
	do {
		getPortName();
		baudRate = getBaudRate();
		printf("Connecting to %s with a Baud rate of %u...\n", portName, baudRate);
		comPort.connectToPort(portName, baudRate);
		printError(comPort.error);
	} while (!comPort.IsConnected() || comPort.error != ERROR_SUCCESS);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(1366 / 2.0, 768 / 2);
	glutInitWindowPosition(0, 0);
	pitchWindow = glutCreateWindow("Pitch Angles");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(1366 / 2, 0);
	glutInitWindowSize(1366 / 2.0, 768 / 2);
	rollWindow = glutCreateWindow("Roll Angles");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(0, 768 / 2);
	glutInitWindowSize(1366 / 3.0, 768 / 2);
	remoteWindow = glutCreateWindow("Remote Control Data");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(1366 / 3, 768 / 2);
	glutInitWindowSize(1366 / 3.0, 768 / 2);
	angleWindow = glutCreateWindow("Corrected Angles");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowPosition(2 * 1366 / 3, 768 / 2);
	glutInitWindowSize(1366 / 3.0, 768 / 2);
	motorsWindow = glutCreateWindow("Motor Output");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);
	
	glutMainLoop();
	return 0;
};

void window() {
	if (getValues() != 0) {
		return;
	};

	bool clear = false;
	previousXPos = xPos;
	xPos += 0.05;
	if (xPos > 5) {
		clear = true;
		xPos = previousXPos = -5;
	}

	glutSetWindow(pitchWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawPitch();
	glutSwapBuffers();

	glutSetWindow(rollWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawRoll();
	glutSwapBuffers();

	glutSetWindow(remoteWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawRemote();
	glutSwapBuffers();
	
	glutSetWindow(angleWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawAngles();
	glutSwapBuffers();

	glutSetWindow(motorsWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawMotors();
	glutSwapBuffers();
}

void onReshape(int _width, int _height) {
	glViewport(0, 0, _width, _height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)_width / _height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
};

int getValues() {
	std::string line;
	try {
		line = comPort.getLine();
		if (line == "" || line.find(',') == -1) {
			if (comPort.error == ERROR_BAD_COMMAND || !comPort.IsConnected()) {
				printf("Lost connection to %s.\nReconnecting...", portName);
				comPort.connectToPort(portName, baudRate);
				printError(comPort.error);
				Sleep(1000);
			}
			return -1;
		}
		
		printf("%s\n", line.c_str());
		std::string data[20];
		for (byte i = 0; i < 20; i++) {
			data[i] = line.substr(0, line.find(','));
			line = line.substr(line.find(',') + 1);
		}
		ax = stoi(data[0]);
		ay = stoi(data[1]);
		az = stoi(data[2]);
		gx = stoi(data[3]);
		gy = stoi(data[4]);
		gz = stoi(data[5]);
		arduinoPitch = stod(data[6]);
		arduinoRoll = stod(data[7]);
		frontLeft = stoi(data[8]);
		frontRight = stoi(data[9]);
		backLeft = stoi(data[10]);
		backRight = stoi(data[11]);
		throttleChannelValue = stoi(data[12]);
		pitchChannelValue = stoi(data[13]);
		rollChannelValue = stoi(data[14]);
		yawChannelValue = stoi(data[15]);
		throttle = stoi(data[16]);
		pitch = stoi(data[17]);
		roll = stoi(data[18]);
		yaw = stoi(data[19]);
		//printf("Line: %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\n", ax, ay, az, gx, gy, gz, frontLeft, frontRight, backLeft, backRight, throttleChannel, pitchChannel, rollChannel, yawChannel, throttle, pitch, roll, yaw);
		return 0;
	} catch (...) {
		printf("Error!");
		return -1;
	}
}

void drawPitch() {
	accelPitch = atan2(ax, sqrt((long)ay*ay + (long)az*az)) * 3.0 * radToDeg;
	gyroPitch = gyroPitch + timeConstant * gy / gyroSensitivity;
	compPitch = compCoeff*(compPitch + timeConstant * gy / gyroSensitivity) + (1.0 - compCoeff)*accelPitch; 

	value = map(arduinoPitch, -90, 90, -2.8, 2.8);
	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousArduinoPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousArduinoPitch = value;

	value = map(compPitch, -90, 90, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousCompPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousCompPitch = value;

	value = map(accelPitch, -90, 90, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousAccelPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousAccelPitch = value;

	value = map(gyroPitch, -90, 90, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousGyroPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousGyroPitch = value;
};

void drawRoll() {
	accelRoll = atan2(ay, sqrt((long)ax*ax + (long)az*az)) * 3 * radToDeg;
	gyroRoll = gyroRoll + timeConstant * -gx / gyroSensitivity;
	compRoll = compCoeff*(compRoll + (timeConstant * -gx / gyroSensitivity)) + (1.0 - compCoeff)*accelRoll;

	value = map(arduinoRoll, -90, 90, -2.8, 2.8);
	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousArduinoRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousArduinoRoll = value;

	value = map(compRoll, -90, 90, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousCompRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousCompRoll = value;

	value = map(accelRoll, -90, 90, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousAccelRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousAccelRoll = value;

	value = map(gyroRoll, -90, 90, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousGyroRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousGyroRoll = value;
};

void drawRemote() {
	value = map(throttleChannel, throttleLowLimit, throttleHighLimit, -2.8, 2.8);
	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousThrottleChannel, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousThrottleChannel = value;

	value = map(pitchChannel, radioLowLimit, radioHighLimit, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousPitchChannel, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousPitchChannel = value;

	value = map(rollChannel, radioLowLimit, radioHighLimit, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousRollChannel, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousRollChannel = value;

	value = map(yawChannel, radioLowLimit, radioHighLimit, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousYawChannel, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousYawChannel = value;
};

void drawAngles() {
	value = map(throttle, escLowLimit, escHighLimit, -2.8, 2.8);
	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousThrottle, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousThrottle = value;

	value = map(pitch, -pitchMaxValue, pitchMaxValue, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousPitch = value;

	value = map(roll, -rollMaxValue, rollMaxValue, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousRoll = value;

	value = map(yaw, -yawMaxValue, yawMaxValue, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousYaw, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousYaw = value;
};

void drawMotors() {
	value = map(frontLeft, escLowLimit, escHighLimit, -2.8, 2.8);
	glColor3d(1, 1, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousFrontLeft, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousFrontLeft = value;

	value = map(frontRight, escLowLimit, escHighLimit, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousFrontRight, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousFrontRight = value;

	value = map(backLeft, escLowLimit, escHighLimit, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousBackLeft, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousBackLeft = value;

	value = map(backRight, escLowLimit, escHighLimit, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousXPos, previousBackRight, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousBackRight = value;
};

void getPortName() {
	printf("Please enter a COM port >");
	scanf_s("%5s", portName, sizeof(portName));
}

int getBaudRate() {
	uint32_t baudRate;
	printf("Please enter the Baud rate >");
	scanf_s("%u", &baudRate);
	return baudRate;
}

void printError(DWORD error) {
	switch (comPort.error) {
	case ERROR_SUCCESS:
		printf("Successfully connected to %s!\n", portName);
		break;
	case ERROR_FILE_NOT_FOUND:
		printf("Error: Handle is not attached because %s is not found.\n", portName);
		break;
	case ERROR_PATH_NOT_FOUND:
		printf("Error: Path to %s is not found.\n", portName);
		break;
	case ERROR_TOO_MANY_OPEN_FILES:
		printf("Error: Cannot connect to %s, too many open files.\n", portName);
		break;
	case ERROR_ACCESS_DENIED:
		printf("Error: Could not get access to %s.\n", portName);
		break;
	default:
		printf("Error: Windows error code: 0x%x\n", comPort.error);
	}
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}