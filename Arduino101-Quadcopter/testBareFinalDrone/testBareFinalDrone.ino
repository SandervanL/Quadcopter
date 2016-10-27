#include "config.h"
#include <MoveMeter.h>        //Self-written class for the pitch and roll (depends on CurieImu)
#include <Calculator.h>       //Library for PID calculations
#include <Motors.h>           //Library for output to motors

//Radio control variables
volatile uint32_t signals[8];     //Array which contains the signals of the remote control in microseconds (updated by ISR(PCINT_vect))
volatile uint32_t signalBegin[8]; //Array with beginvalues of the remote-control signals

//Chip, calculation and motor variables
Calculator    calculator;         //Instance of the PID library
Motors        motors;             //Instance of the library which controls the motors
MoveMeter     moveMeter;          //Instance of the MPU

//
double mpuPitch, mpuRoll;         //Variables for the mpu and height, we store lastHeight for our algorithm in loop()
uint16_t throttle;
int16_t pitch, roll, yaw;         //The final output variables throttle in microseconds
byte startState = 0;              //The startState determines if the quad is ready to start (throttle low and yaw left = 1, throttle low and yaw middle = 2 = begin flying)
uint32_t beginTimeOfNextLoop = 0; //The loop() function has to take a certain amount of time (timeConstant in config.h). While it hasn't completed it's cycle, it will wait for the time to be this value

#ifdef DEBUG
byte timePortState = 0; //An even portState indicates on, odd indicates off.
#endif

void setup() {
#ifdef DEBUG
  Serial.begin(9600);                       //Initialize serial bus if we want to debug our code
  while (!Serial);
  Serial.println("Online!");
  pinMode(timePort, OUTPUT);
#endif
  pinMode(warningLED, OUTPUT);
   
  //If the CurieIMU is connected, go on. Else, blink the warning LED.
  while (!moveMeter.testConnection()) {
#ifdef DEBUG
    Serial.println("CurieIMU not found!");
#endif
    digitalWrite(warningLED, HIGH); //Turn on warningLED
    delay(250);
    digitalWrite(warningLED, LOW); //Turn off warningLED
  }
#ifdef DEBUG
  Serial.println("CurieIMU found!");
#endif

  //Initialize all the instances
  motors.initialize();
  moveMeter.initialize();
  calculator.resetAll();

  //Enable the pull-up resistor for better values
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);

  //Setup the interrupts to watch the remote-control values change
  attachInterrupt(A0, A0Interrupt, CHANGE);
  attachInterrupt(A1, A1Interrupt, CHANGE);
  attachInterrupt(A2, A2Interrupt, CHANGE);
  attachInterrupt(A3, A3Interrupt, CHANGE);
  attachInterrupt(A4, A4Interrupt, CHANGE);
  attachInterrupt(A5, A5Interrupt, CHANGE);
};



void loop() {
  //Wait a while before beginning the next loop, so the I component of the PID won't get inequal values
  while (micros() < beginTimeOfNextLoop) {
    delayMicroseconds(2);
  }
  beginTimeOfNextLoop = micros() + (uint32_t)(timeConstant * 1000000.0); //Set time next loop can start
  
#ifdef DEBUG
  //Toggle timePort so the cycle length can be measured with an oscilloscope.
  digitalWrite(timePort, ++timePortState % 2);
#endif

  //If the throttle stick is low and yaw is left, we are going to wait for it to go to the middle
  if (startState == 0 && signals[throttleChannel] < 1080 && signals[yawChannel] < 1080 && signals[yawChannel] >= 1040 && signals[throttleChannel] >= 1040) {
    startState = 1;
    //If the throttle stick is low and yaw is in the middle and yaw has been left, we start the motors and we are ready to fly.
  } else if (startState == 1 && signals[throttleChannel] < 1080 && signals[throttleChannel] >= 1040 && signals[yawChannel] > 1400) {
    startState = 2;
    motors.startAll();
    calculator.resetAll();
    //If the throttle stick is low and yaw is right, the motors are shutdown and startState is again 0 (start over)
  } else if (startState == 2 && signals[throttleChannel] < 1080 && signals[throttleChannel] >= 1040 && signals[yawChannel] > 1850) {
    startState = 0;//By setting startState to 0, the motors are stopped at the end of the loop
  };

  moveMeter.getPitchRoll(mpuPitch, mpuRoll);   //Get pitch and roll (in degrees)

  //Calculate the throttle output for the motors. Keep a band (escHighLimit - 200) as a buffer for pitch, roll and yaw
  throttle =  (uint16_t) map(signals[throttleChannel], throttleLowLimit, throttleHighLimit, escLowLimit, escHighLimit - 200);

  //Only compute PID when we have input; we don't want the PID to keep trying to correct while we don't output any signal
  //I don't use return, because I want to remain collecting data from the IMU and barometer.
  if (startState == 2 && throttle > 1080) {
    calculator.PIDPitchRoll(signals, mpuPitch, mpuRoll, pitch, roll); //Compute the wanted pitch and roll output    
    yaw = ((signals[yawChannel] - radioLowLimit) * (2 * yawMaxValue) / (radioHighLimit - radioLowLimit) - yawMaxValue);

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
  Serial.print(mpuPitch);                     Serial.print(",");
  Serial.print(mpuRoll);                      Serial.print(",");
  Serial.print(throttle + pitch - roll + yaw);Serial.print(",");
  Serial.print(throttle + pitch + roll - yaw);Serial.print(",");
  Serial.print(throttle - pitch - roll - yaw);Serial.print(",");
  Serial.print(throttle - pitch + roll + yaw);Serial.print(",");
  Serial.print(signals[throttleChannel]);     Serial.print(",");
  Serial.print(signals[pitchChannel]);        Serial.print(",");
  Serial.print(signals[rollChannel]);         Serial.print(",");
  Serial.print(signals[yawChannel]);          Serial.print(",");
  Serial.print(throttle);                     Serial.print(",");
  Serial.print(pitch);                        Serial.print(",");
  Serial.print(roll);                         Serial.print(",");
  Serial.print(yaw);                          Serial.print(",");
  Serial.println(startState);
#endif
};

void A0Interrupt() {
  if (!signalBegin[0]) {
    signalBegin[0] = micros();
  } else {
    signals[0] = micros() - signalBegin[0];
    signalBegin[0] = 0;
  }
}

void A1Interrupt() {
  if (!signalBegin[1]) {
    signalBegin[1] = micros();
  } else {
    signals[1] = micros() - signalBegin[1];
    signalBegin[1] = 0;
  }
}

void A2Interrupt() {
  if (!signalBegin[2]) {
    signalBegin[2] = micros();
  } else {
    signals[2] = micros() - signalBegin[2];
    signalBegin[2] = 0;
  }
}

void A3Interrupt() {
  if (!signalBegin[3]) {
    signalBegin[3] = micros();
  } else {
    signals[3] = micros() - signalBegin[3];
    signalBegin[3] = 0;
  }
}

void A4Interrupt() {
  if (!signalBegin[4]) {
    signalBegin[4] = micros();
  } else {
    signals[4] = micros() - signalBegin[4];
    signalBegin[4] = 0;
  }
}

void A5Interrupt() {
  if (!signalBegin[5]) {
    signalBegin[5] = micros();
  } else {
    signals[5] = micros() - signalBegin[5];
    signalBegin[5] = 0;
  }
}
