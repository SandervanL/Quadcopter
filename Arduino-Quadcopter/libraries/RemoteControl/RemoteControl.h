#include "Arduino.h"

//Radio control variables, micros() returns a uint32_t
volatile uint16_t signals[8];   //Array which contains the signals of the remote control in microseconds (updated by ISR(PCINT_vect))
volatile uint32_t signalBegin[8] = {0};        //Array with beginvalues of the remote-control signals
volatile uint32_t currentMicroTime;      //Variable used by ISR(PCINT0_vect) to store the current time in


ISR (PCINT1_vect) {
	currentMicroTime = micros();
	//Channel 1
	if (PINC & B00000001 && signalBegin[0] == 0) {  //Input 8 changed from 0 to 1
		signalBegin[0] = currentMicroTime;      	//Remember the beginning of the pulse
	} else if (signalBegin[0]) {               		//Input 8 is not high and changed from 1 to 0
		signals[0] = currentMicroTime - signalBegin[0];//Channel 1 is current time - begin time
		signalBegin[0] = 0;                       	//Remember channel 1 is now low
	};
	//Channel 2
	if (PINC & B00000010 && signalBegin[1] == 0) { 	//Input 9 changed from 0 to 1
		signalBegin[1] = currentMicroTime;    		//Remember the beginning of the pulse
	} else if (signalBegin[1]) {                  	//Input 9 is not high and changed from 1 to 0
		signals[1] = currentMicroTime - signalBegin[1];//Channel 2 is current time - begin time
		signalBegin[1] = 0;                       	//Remember channel 2 is now low
	};
	//Channel 3
	if (PINC & B00000100 && signalBegin[2] == 0) {	//Input 10 changed from 0 to 1
		signalBegin[2] = currentMicroTime;      	//Remember the beginning of the pulse
	} else if (signalBegin[2]) {                	//Input 10 is not high and changed from 1 to 0
		signals[2] = currentMicroTime - signalBegin[2];//Channel 3 is current time - begin time
		signalBegin[2] = 0;                       	//Remember channel 3 is now low
	};
	//Channel 4
	if (PINC & B00001000 && signalBegin[3] == 0) { 	//Input 11 changed from 0 to 1
		signalBegin[3] = currentMicroTime;      	//Remember the beginning of the pulse
	} else if (signalBegin[3]) {                 	//Input 11 is not high and changed from 1 to 0
		signals[3] = currentMicroTime - signalBegin[3];//Channel 4 is current time - begin time
		signalBegin[3] = 0;                       	//Remember channel 4 is now low
	};
}
	
void initializeRemoteControl() {
	//Enable the pull-up resistor for better values
	pinMode(A0, INPUT_PULLUP);
	pinMode(A1, INPUT_PULLUP);
	pinMode(A2, INPUT_PULLUP);
	pinMode(A3, INPUT_PULLUP);
	
	//Check https://www.arduino.cc/en/Reference/PortManipulation for more information
	//Setup the remote-control pins
	PCICR  |= (1 << PCIE1);     //Setup interrupt listening for the analog pins
	PCMSK1 |= (1 << PCINT8);	//Setup PCINT8 (A0) to trigger an interrupt.
	PCMSK1 |= (1 << PCINT9);    //Setup PCINT9 (A1) to trigger an interrupt.
	PCMSK1 |= (1 << PCINT10);   //Setup PCINT10(A2) to trigger an interrupt.
	PCMSK1 |= (1 << PCINT11);  	//Setup PCINT11(A3) to trigger an interrupt.
}