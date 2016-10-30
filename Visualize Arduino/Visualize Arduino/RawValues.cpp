/*#define playRaw
#ifdef playRaw
#include "SerialClass.h"
#include <cstdio>
#include <GL\glew.h>
#include <gl\freeglut.h>
#include <sstream>

static const double gyroSensitivity = 800.0f;
static const double compCoeff = 0.98f;
static const double radToDeg = 57.295779513082320876798154814105;

char portName[5];
int baudRate;
Serial comPort;

void window();
void onReshape(int width, int height);
void getValues();
void drawPitchValues();
void drawRollValues();
double map(double x, double in_min, double in_max, double out_min, double out_max);
int width = 683, height = 384;
int pitchWindow, rollWindow;
double value;

int main(int argc, char *argv[]) {
	do {
		printf("Please enter a COM port >");
		scanf_s("%s", portName, sizeof(portName));
		printf("Please enter the baud rate>");
		scanf_s("%d", &baudRate);
		printf("Opening port %s at a baud rate of %d...\n", portName, baudRate);
		comPort.connectToPort(portName, baudRate);
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
	} while (!comPort.IsConnected() || comPort.error != ERROR_SUCCESS);

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	pitchWindow = glutCreateWindow("Pitch window");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(width, 0);
	rollWindow = glutCreateWindow("Roll window");
	glutIdleFunc(window);
	glutDisplayFunc(window);
	glutReshapeFunc(onReshape);

	glutMainLoop();
	return 0;
}

/* X width: 10 (-5 ~ 5);
Y width: 5,6 (-2,8 ~ 2,8)
Z depth: -5


GLfloat xPos = -5.0f;
GLfloat previousxPos = -5.0f;
bool clear = false;
void window() {
	previousxPos = xPos;
	xPos += 0.03;
	if (xPos >= 5) {
		clear = true;
		printf("Reset!");
		xPos = previousxPos = -5.0f;
	}
	getValues();

	glutSetWindow(pitchWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawPitchValues();
	glutSwapBuffers();

	glutSetWindow(rollWindow);
	if (clear) {
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	glLoadIdentity();
	drawRollValues();
	glutSwapBuffers();

	clear = false;
}

void onReshape(int _width, int _height) {
	width = _width;
	height = _height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (float)width / height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
};

int ax, ay, az, gx, gy, gz;
double dt;
void getValues() {
	std::string values[14];
	while (true) {
		try {
			std::string line = comPort.getLine();
			if (line.compare("") == 0) {
				xPos -= 0.03;
				previousxPos -= 0.03;
				if (comPort.error == ERROR_BAD_COMMAND || !comPort.IsConnected()) {
					Sleep(1000);
					printf("Lost connection to %s.\nReconnecting...\n", portName);
					comPort.connectToPort(portName, baudRate);
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
				break;
			}
			std::istringstream lineStream(line);
			printf("Line: %s\n", lineStream.str().c_str());
			std::string s;
			int i = -1;
			while (std::getline(lineStream, s, ',')) {
				if (++i >= 7) {
					throw 1;
				}
				values[i] = s;
			}
			ax = stoi(values[0]);
			ay = stoi(values[1]);
			az = stoi(values[2]);
			gx = stoi(values[3]);
			gy = stoi(values[4]);
			gz = stoi(values[5]);
			dt = stoi(values[6]) / 1000000.0;
			break;
		} catch (...) {
			printf("ERROR!");
		};
	};
}

double pitch, accelPitch, gyroPitch;
double previousPitch, previousAccelPitch, previousGyroPitch;
void drawPitchValues() {
	accelPitch = atan2(ax, sqrt((long)ay*ay + (long)az*az)) * 3 * radToDeg;
	gyroPitch = gyroPitch + dt * -gy / gyroSensitivity;
	pitch = compCoeff*(pitch + (dt * -gy / gyroSensitivity)) + (1.0 - compCoeff)*accelPitch;

	/*value = map(pitch, -90, 90, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousPitch = value;//Asterisk here

	value = map(accelPitch, -90, 90, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousAccelPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousAccelPitch = value;

	/*value = map(gyroPitch, -90, 90, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousGyroPitch, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousGyroPitch = value;//Asterisk here
}

double accelRoll, gyroRoll, roll;
double previousAccelRoll, previousGyroRoll, previousRoll;
void drawRollValues() {
	accelRoll = atan2(ay, sqrt((long)ax*ax + (long)az*az))* 3 * radToDeg;
	gyroRoll = gyroRoll + dt * -gx / gyroSensitivity;
	roll = compCoeff*(roll + (dt * -gx / gyroSensitivity)) + (1.0 - compCoeff)*accelRoll;
	
	/*value = map(roll, -90, 90, -2.8, 2.8);
	glColor3d(1, 0, 0);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousRoll = value;//Asterisk here

	value = map(accelRoll, -90, 90, -2.8, 2.8);
	glColor3d(0, 1, 0);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousAccelRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousAccelRoll = value;

	/*value = map(gyroRoll, -90, 90, -2.8, 2.8);
	glColor3d(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(previousxPos, previousGyroRoll, -5);
	glVertex3f(xPos, value, -5);
	glEnd();
	previousGyroRoll = value;//Asterisk here
}

double map(double x, double in_min, double in_max, double out_min, double out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
#endif*/