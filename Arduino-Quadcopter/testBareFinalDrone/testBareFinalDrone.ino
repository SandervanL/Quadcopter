//#define DEBUGo
#include "config.h"        //File containing all the constants needed to run
#include <Wire.h>             //Dumb I2C library
#include <I2Cdev.h>           //Smart I2C library (depends on Wire.h)
#include <MPU6050.h>          //Library for the imu
#include <MoveMeter.h>        //Self-written class for the pitch and roll (depends on MPU6050 and AK8975)
#include <Calculator.h>       //Library for PID calculations
#include <Motors.h>           //Library for output to motors


//Radio control variables, micros() returns a uint32_t
volatile uint16_t signals[8];   //Array which contains the signals of the remote control in microseconds (updated by ISR(PCINT_vect))
volatile uint32_t signalBegin[8];        //Array with beginvalues of the remote-control signals
volatile uint32_t currentMicroTime;      //Variable used by ISR(PCINT0_vect) to store the current time in

//Chip, calculation and motor variables
Calculator    calculator;           //Instance of the PID library
Motors        motors;               //Instance of the library which controls the motors
MoveMeter     moveMeter;            //Instance of the MPU

double mpuPitch, mpuRoll;             //Variables for the mpu and height, we store lastHeight for our algorithm in loop()
uint32_t lastTime;                     //To store the time the last loop ended
uint16_t throttle;
int16_t pitch, yaw, roll;                           //The final output variables throttle in microseconds, pitch in PID, yaw in (% - 50), roll in PID
uint8_t startState = 0;                                      //The startState determines if the quad is ready to start (throttle low and yaw left = 1, throttle low and yaw middle = 2 = begin flying)
uint32_t beginTimeOfNextLoop = 0;     //Stores the time at which the next loop should start

void setup() {
#ifdef DEBUG
  Serial.begin(9600);                       //Initialize serial bus if we want to debug our code
  Serial.println("Online!");
#endif
  Wire.begin();                               //Initialize I2C bus
  Wire.setClock(400000L);                    //Set speed of I2C bus to fastest possible
  analogReference(INTERNAL);
  pinMode(warningLED, OUTPUT);
  pinMode(timePort, OUTPUT);

  //If the MPU-9150 is connected, go on. Else, blink the warning LED.
  while (!moveMeter.testMPUConnection()) {
    digitalWrite(warningLED, !digitalRead(warningLED)); //Toggle warningLED
#ifdef DEBUG
    Serial.println("MPU-9150 not found!");
#endif
    delay(250);
  }
#ifdef DEBUG
  Serial.println("MPU-9150 found!");
#endif

  //Initialize all the instances
  motors.initialize();
  moveMeter.initialize();
  calculator.resetPitchRollParameters();

  //Enable the pull-down resistor for better values
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);

  //Check https://www.arduino.cc/en/Reference/PortManipulation for more information
  //Setup the remote-control pins
  PCICR  |= (1 << PCIE1);        //Setup interrupt listening for the analog pins
  PCMSK1 |= (1 << PCINT8);              //Setup PCINT8 (A0) to trigger an interrupt.
  PCMSK1 |= (1 << PCINT9);              //Setup PCINT9 (A1) to trigger an interrupt.
  PCMSK1 |= (1 << PCINT10);             //Setup PCINT10(A2) to trigger an interrupt.
  PCMSK1 |= (1 << PCINT11);             //Setup PCINT11(A3) to trigger an interrupt.
};



void loop() {
  //If timePort is low (digitalRead(timePort)), we set timePort high (PINx | (1 << PINx)), else we set the port low (PORTx | (1 << PB4))
  //Now, timePort = PB4 = 12
  while (micros() < beginTimeOfNextLoop) {
    delayMicroseconds(2);
  }
  beginTimeOfNextLoop = micros() + (uint32_t)(timeConstant * 1000000.0);
  PORTB = PINB & (1 << PB4) ? PORTB & ~(1 << PB4) : PORTB | (1 << PB4);

  //If the throttle stick is low and yaw is left, we are going to wait for it to go to the middle
  if (startState == 0 && signals[throttleChannel] < 1080 && signals[yawChannel] < 1080 && signals[yawChannel] >= 1040 && signals[throttleChannel] >= 1040) {
    startState = 1;
    //If the throttle stick is low and yaw is in the middle and yaw has been left, we start the motors and we are ready to fly.
  } else if (startState == 1 && signals[throttleChannel] < 1080 && signals[throttleChannel] >= 1040 && signals[yawChannel] > 1400) {
    startState = 2;
    calculator.resetAll();
    //If the throttle stick is low and yaw is right, the motors are shutdown and startState is again 0 (start over)
  } else if (startState == 2 && signals[throttleChannel] < 1080 && signals[throttleChannel] >= 1040 && signals[yawChannel] > 1850) {
    startState = 0; //By setting startState to 0, the motors are stopped at the end of the loop
  };

  moveMeter.getPitchRoll(&mpuPitch, &mpuRoll);//Get pitch and roll (in degrees)
  mpuPitch *= -1.0;                             //Compensate for placing the MPU9150 the other way around
  mpuRoll  *= -1.0;

  //escLowLimit - 200 to have some free space for the roll, pitch and yaw
  throttle = (uint16_t) ((signals[throttleChannel] - throttleLowLimit) * (double)(escHighLimit - 200 - escLowLimit) / (throttleHighLimit - throttleLowLimit) + escLowLimit);
  
  //I don't use return, because I may want to DEBUG
  if (startState == 2 && throttle > 1080) {
    //Only compute PID when we have input; we don't want the PID to keep trying to correct while we don't output any signal
    calculator.PIDPitchRoll(signals, mpuPitch, mpuRoll, pitch, roll); //Calculate the wanted pitch and roll output

    //Calculate the throttle and yaw from the remote control
    //throttleChannel, throttleLowLimit, throttleHighLimit, yawChannel, radioLowLimit and radioHighLimit are defined in Calculator.h
    //escLowLimit and escHighLimit are defined in Motors.h
    yaw = (signals[yawChannel] - radioLowLimit) * (2.0 * yawMaxValue) / (radioHighLimit - radioLowLimit) - yawMaxValue;

    //Checking if throttle and yaw are within the specified range. Else, adjust. The specified range is yawMaxValue defined in Calculator.h
    if (throttle > escHighLimit - 200) {
      throttle = escHighLimit - 200;
    }
    if (yaw > yawMaxValue) {
      yaw = yawMaxValue;
    } else if (yaw < yawMaxValue * -1) {
      yaw = yawMaxValue * -1;
    }
    motors.outputAll(throttle, pitch, roll, yaw);
  } else {
    motors.staySilent();
  }
#ifdef DEBUG
  //Print all the data which is just collected to the serial monitor on the computer
  //MPUPitch and MPURoll are taken care of in the MoveMeter library
  Serial.print(mpuPitch);                     Serial.print(",");
  Serial.print(mpuRoll);                      Serial.print(",");
  Serial.print(throttle + Pitch - Roll + Yaw);Serial.print(",");
  Serial.print(throttle + Pitch + Roll - Yaw);Serial.print(",");
  Serial.print(throttle - Pitch - Roll - Yaw);Serial.print(",");
  Serial.print(throttle - Pitch + Roll + Yaw);Serial.print(",");
  Serial.print(signals[throttleChannel]);     Serial.print(",");
  Serial.print(signals[pitchChannel]);        Serial.print(",");
  Serial.print(signals[rollChannel]);         Serial.print(",");
  Serial.print(signals[yawChannel]);          Serial.print(",");
  Serial.print(throttle);                     Serial.print(",");
  Serial.print(Pitch);                        Serial.print(",");
  Serial.print(Roll);                         Serial.print(",");
  Serial.print(Yaw);                          Serial.print(",");
  Serial.println(startState);
#endif
};

ISR (PCINT1_vect) {
  currentMicroTime = micros();
  //Channel 1
  if ((PINC & B00000001) == 0) {              //Is input 8 high?
    if (!signalBegin[0]) {                    //Input 8 changed from 0 to 1
      signalBegin[0] = currentMicroTime;      //Remember the beginning of the pulse
    };
  } else if (signalBegin[0]) {               //Input 8 is not high and changed from 1 to 0
    signals[0] = currentMicroTime - signalBegin[0]; //Channel 1 is current time - begin time
    signalBegin[0] = 0;                       //Remember channel 1 is now low
  };
  //Channel 2
  if ((PINC & B00000010) == 0) {              //Is input 9 high?
    if (!signalBegin[1]) {                    //Input 9 changed from 0 to 1
      signalBegin[1] = currentMicroTime;      //Remember the beginning of the pulse
    };
  } else if (signalBegin[1]) {                //Input 9 is not high and changed from 1 to 0
    signals[1] = currentMicroTime - signalBegin[1];//Channel 2 is current time - begin time
    signalBegin[1] = 0;                       //Remember channel 2 is now low
  };
  //Channel 3
  if ((PINC & B00000100) == 0) {              //Is input 10 high?
    if (!signalBegin[2]) {                    //Input 10 changed from 0 to 1
      signalBegin[2] = currentMicroTime;      //Remember the beginning of the pulse
    };
  } else if (signalBegin[2]) {                //Input 10 is not high and changed from 1 to 0
    signals[2] = currentMicroTime - signalBegin[2];//Channel 3 is current time - begin time
    signalBegin[2] = 0;                       //Remember channel 3 is now low
  };
  //Channel 4
  if ((PINC & B00001000) == 0) {              //Is input 11 high?
    if (!signalBegin[3]) {                    //Input 11 changed from 0 to 1
      signalBegin[3] = currentMicroTime;      //Remember the beginning of the pulse
    };
  } else if (signalBegin[3]) {                 //Input 11 is not high and changed from 1 to 0
    signals[3] = currentMicroTime - signalBegin[3];//Channel 4 is current time - begin time
    signalBegin[3] = 0;                       //Remember channel 4 is now low
  };
}
