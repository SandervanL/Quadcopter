#include "config.h"        //File containing all the constants needed to run
#include <Wire.h>             //Dumb I2C library
#include <I2Cdev.h>           //Smart I2C library (depends on Wire.h)
#include <MPU6050.h>          //Library for the imu
#include <MoveMeter.h>        //Self-written class for the pitch and roll (depends on MPU6050 and AK8975)
#include <Calculator.h>       //Library for PID calculations
#include <Motors.h>           //Library for output to motors
#include <RemoteControl.h>    //Library for remote control

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
  Serial.begin(38400);                       //Initialize serial bus if we want to debug our code
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
  initializeRemoteControl();
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

  moveMeter.getPitchRoll(mpuPitch, mpuRoll);//Get pitch and roll (in degrees)
  mpuPitch *= -1.0;                             //Compensate for placing the MPU9150 the other way around
  mpuRoll  *= -1.0;
  
  //I don't use return, because I may want to DEBUG
  if (startState == 2 && signals[throttleChannel] > 1080) {
    //Only compute PID when we have input; we don't want the PID to keep trying to correct while we don't output any signal
    calculator.PIDPitchRoll(signals, mpuPitch, mpuRoll, pitch, roll); //Calculate the wanted pitch and roll output

    //Calculate the throttle and yaw from the remote control
    //throttleChannel, throttleLowLimit, throttleHighLimit, yawChannel, radioLowLimit and radioHighLimit are defined in Calculator.h
    //escLowLimit and escHighLimit are defined in Motors.h
    //escLowLimit - 200 to have some free space for the roll, pitch and yaw
    throttle = (uint16_t) ((signals[throttleChannel] - throttleLowLimit) * (double)(escHighLimit - 200 - escLowLimit) / (throttleHighLimit - throttleLowLimit) + escLowLimit);
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
  //MPUPitch and MPURoll are taken care of in the MoveMeter library, the motor values are taken care of in the Motor library
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
