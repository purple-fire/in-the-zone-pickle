/**
 * File for operator control code
 *
 * This file should contain the user operatorControl() function and any
 * functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "liftControl.h"
#include "main.h"
#include "pid.h"
#include "utilities.h"
#include "motor.h"

typedef enum { DRIVE_AUTO, DRIVE_TANK, DRIVE_ARCADE } DriveMode;

DriveMode driveMode;
int liftPosition;
/**
 * Driver control with tank-style controls.
 */
static void driverControl(void *parameter) {
  chassisStop();

  while (true) {

    if (driveMode == DRIVE_TANK) {
      int joyRight = joystickGetAnalog(1, CRY);
      int joyLeft = joystickGetAnalog(1, CLY);

      if (ABS(joyRight) <= 10) {
        rightMotorsBrake();
      } else {
        rightMotorsSet(joyRight * MOTOR_POWER_MAX / 127);
      }

      if (ABS(joyLeft) <= 10) {
        leftMotorsBrake();
      } else {
        leftMotorsSet(joyLeft * MOTOR_POWER_MAX / 127);
      }

    } else if (driveMode == DRIVE_ARCADE) {
      int joyRight = joystickGetAnalog(1, CRX);
      int joyLeft = joystickGetAnalog(1, CLY);

      int leftPower = joyLeft + joyRight;
      int rightPower = joyLeft - joyRight;

      int motorMAX = MOTOR_POWER_MAX;

      if(leftPower!=rightPower){
        motorMAX = 100;
      }

      if (ABS(leftPower) <= 8) {
        leftMotorsBrake();
      } else {
        leftMotorsSet(leftPower * motorMAX / 127);
      }

      if (ABS(rightPower) <= 8) {
        rightMotorsBrake();
      } else {
        rightMotorsSet(rightPower * motorMAX / 127);
      }

    } else {
      /* Stop for invalid modes */
      chassisStop();
    }

    // Motor values can only be updated every 20ms
    delay(20);
  }
}

/*
static void startAutoPilot(void *parameter) {
  chassisStopSmooth();
  autonomous();
}
*/
void operatorControl() {


  TaskHandle driverControlHandle;
  //TaskHandle autoPilotHandle, driverControlHandle;
  driveMode = DRIVE_ARCADE;
  driverControlHandle = taskCreate(driverControl, TASK_DEFAULT_STACK_SIZE, NULL,
                                   TASK_PRIORITY_DEFAULT);

  setMogoAngle(MOGO_UP);
  mogoToggle = 1;
  setConeAngle(CONE_UP);

  int toggleCone = 0;//,toggleLift,toggleMogo;

  while (true) {
    //motorSet(5,-127); //SET PIN 5 HIGH


    if (driveMode != DRIVE_AUTO && (joystickGetDigital(1, 7, JOY_LEFT) == 1)) {
      taskDelete(driverControlHandle);
      //autoPilotHandle = taskCreate(startAutoPilot, TASK_DEFAULT_STACK_SIZE,
      //                             NULL, TASK_PRIORITY_DEFAULT);
      driveMode = DRIVE_AUTO;
    }else if (joystickGetDigital(1, 7, JOY_RIGHT) == 1) {
      if (driveMode == DRIVE_AUTO) {
        //taskDelete(autoPilotHandle);
        driverControlHandle = taskCreate(driverControl, TASK_DEFAULT_STACK_SIZE,
                                         NULL, TASK_PRIORITY_DEFAULT);
      }
      driveMode = DRIVE_ARCADE;
    }

    if (driveMode != DRIVE_AUTO) {

      if (joystickGetDigital(1, 6, JOY_UP) == 1) {
        setMogoAngle(MOGO_UP);
      } else if (joystickGetDigital(1, 6, JOY_DOWN) == 1) {
        setMogoAngle(MOGO_DOWN);

      } else if (joystickGetDigital(1, 5, JOY_DOWN) == 1 || joystickGetDigital(1, 5, JOY_UP) == 1) {
        setMogoAngle(MOGO_HALF);
      }


//Automated controls for the cone arm

//Toggle Cone states:
//Top:  1
//Mid:  0
//Btm: -1

//Cone arm is at the bottom. Press RIGHT to go to middle.
      if ((joystickGetDigital(1, 8, JOY_RIGHT) == 1)&&(toggleCone==-1)) {
        setConeAngle(CONE_HALF);
        toggleCone = 0;

      }
//Cone arm is at the middle. press RIGHT to go to up.
      else if ((joystickGetDigital(1, 8, JOY_RIGHT) == 1)&&(toggleCone==0)) {
        setConeAngle(CONE_UP);
        toggleCone = 1;

//Cone arm is at the middle. Press LEFT to have the arm bob down, pick up a cone, and bob back up to middle.
}      else if ((joystickGetDigital(1, 8, JOY_LEFT) == 1)&&(toggleCone==0)) {
        setConeAngle(CONE_DOWN);

        while (joystickGetDigital(1, 8, JOY_LEFT) == 1){
          motorSet(goliathMotor,GOLIATH_IN);
        }
        setConeAngle(CONE_HALF);
        motorSet(goliathMotor,0);


//Cone arm is at the top Press LEFT to go to middle or HOLD LEFT to go to bottom.
}else if (joystickGetDigital(1, 8, JOY_LEFT) == 1 && toggleCone==1)  {

        setConeAngle(CONE_HALF);
        toggleCone = 0;

      }

else{

    }
      if (joystickGetDigital(1, 8, JOY_UP) == 1) {
        motorSet(liftMotor,-127);
        motorSet(liftMotorAux,-127);
      } else if (joystickGetDigital(1, 8, JOY_DOWN) == 1) {
        motorSet(liftMotor,127);
        motorSet(liftMotorAux,127);
      }
      else{
        motorSet(liftMotor,0);
        motorSet(liftMotorAux,0);
      }

      if (joystickGetDigital(1, 7, JOY_UP) == 1) {
        motorSet(goliathMotor,GOLIATH_IN);
      } else if (joystickGetDigital(1, 7, JOY_DOWN) == 1) {
        motorSet(goliathMotor,GOLIATH_OUT);
      }
      else{
        motorSet(goliathMotor,0);
      }

    }
    delay(50);
  }
  //taskDelete(autoPilotHandle);
  taskDelete(driverControlHandle);

}
