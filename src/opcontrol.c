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

int liftMode = 0;
int steps;

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
                motorMAX = 127;
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

    setMogoAngle(analogRead(MOGO_POT_PORT));
    mogoToggle = 1;
    setConeAngle(analogRead(CONE_POT_PORT));

    int toggleCone = 0;//,toggleLift,toggleMogo;

    while (true) {
        //motorSet(5,-127); //SET PIN 5 HIGH


        if (LIFT_CONTROL_SWITCH == 1){
            if (liftMode == 0){

                liftMode = 1;
                steps = 0;
                resetCones();
                setConeAngle(CONE_UP);
                printf("Toggling to auto control...");
                delay (500);

            }

            /*
               else if (liftMode == 1){
               liftMode = 0;
               printf("Toggling to manual control...");
               delay(500);
               }

*/
        }


        if (driveMode != DRIVE_AUTO && (MODE_SWITCH_LEFT_BUTTON == 1)) {
            taskDelete(driverControlHandle);
            //autoPilotHandle = taskCreate(startAutoPilot, TASK_DEFAULT_STACK_SIZE,
            //                             NULL, TASK_PRIORITY_DEFAULT);
            driveMode = DRIVE_AUTO;
        }else if (MODE_SWITCH_RIGHT_BUTTON == 1) {
            if (driveMode == DRIVE_AUTO) {
                //taskDelete(autoPilotHandle);
                driverControlHandle = taskCreate(driverControl, TASK_DEFAULT_STACK_SIZE,
                        NULL, TASK_PRIORITY_DEFAULT);
            }
            driveMode = DRIVE_ARCADE;
        }

        if (driveMode != DRIVE_AUTO) {

            if (MOGO_UP_BUTTON == 1) {
                setMogoAngle(MOGO_UP);
            } else if (MOGO_DOWN_BUTTON == 1) {

                setMogoAngle(MOGO_DOWN);
                if (liftMode == 1){
                    resetCones();
                }



            } else if (MOGO_DROP_LOW_BUTTON == 1) {
                setMogoAngle(MOGO_DROP_LOW);
            }

        } else if (MOGO_DROP_HIGH_BUTTON == 1) {
            setMogoAngle(MOGO_DROP_HIGH);
        }
        //Automated controls for the cone arm

        //Toggle Cone states:
        //Top:  1
        //Mid:  0
        //Btm: -1

        //MANUAL LIFT CONTROL

        if (liftMode == 0){
            //Cone arm is at the bottom. Press RIGHT to go to middle.
            if ((CONE_ARM_UP_BUTTON == 1)&&(toggleCone==1)) {
                setConeAngle(CONE_UP);
                toggleCone = 0;

            }

            //Cone arm is at the middle. press RIGHT to go to up.
            else if ((CONE_ARM_UP_BUTTON == 1)&&(toggleCone==0)) {
                setConeAngle(CONE_DOWN);
                toggleCone = 1;
            }

            else if ((CONE_ARM_DOWN_BUTTON == 1)&&(grabState==GRABBED_STACK)) {
                ungrabStack();
                pickupCone(1);
            }

            else if (CONE_ARM_DOWN_BUTTON) {
                switch (grabState) {
                case GRABBED_STACK:
                    /* Cone arm is holding the top of the stack
                     * Move off the stack and drop down to pick up a cone
                     */
                    ungrabStack();
                    pickupCone(1);
                    break;

                case GRABBED_NONE:
                case GRABBED_CONE:
                    /* Cone arm is not holding the top of the stack
                     * Just pick up a cone
                     */
                    pickupCone(1);
                    break;
                }
            }

            //RD4B Controls
            //Semi-automated stacking where manual control over the lift is
            //given, but upon release it autostacks
            if (LIFT_UP_BUTTON) {
                stackCone();

                /*
                motorSet(liftMotor, 0);
                motorSet(liftMotorAux, 0);

                //int originalHeight = analogRead(LIFT_POT_PORT);

                setConeAngle(CONE_UP_OFFSET);
                delay(250);
                motorSet(goliathMotor,GOLIATH_IN);
                // setLiftHeight(originalHeight - 275);
                motorSet(liftMotor,64);
                motorSet(liftMotorAux,64);
                delay(300);
                motorSet(liftMotor,0);
                motorSet(liftMotorAux,0);

                motorSet(goliathMotor,GOLIATH_OUT);
                motorSet(liftMotor,-64);
                motorSet(liftMotorAux,-64);
                delay(300);
                motorSet(liftMotor,0);
                motorSet(liftMotorAux,0);

                motorSet(goliathMotor,0);
                */

            } else if (MANUAL_LIFT_UP_BUTTON == 1){
                liftToggle = 0;
                motorSet(liftMotor,-127);
                motorSet(liftMotorAux,-127);
            } else if (LIFT_DOWN_BUTTON == 1) {
                liftToggle = 0;
                motorSet(liftMotor,127);
                motorSet(liftMotorAux,127);
            } else {
                liftToggle = 1;
                liftTarget = liftPosition;
            }

            //Goliath Controls
            if (GOLIATH_IN_BUTTON  == 1) {
                motorSet(goliathMotor,GOLIATH_IN);
            } else if (GOLIATH_OUT_BUTTON  == 1) {
                motorSet(goliathMotor,GOLIATH_OUT);
            }
            else{
                motorSet(goliathMotor,0);
            }

            if (CONE_DEC_BUTTON) {
                decrementNumCones();
            }
        }

        delay(100);

    } //End of manual control mode


    /*else if (liftMode == 1){

      if (steps == 0 && CONE_ARM_DOWN_BUTTON == 1){
      pickupCone(1);
      steps = 1;
      delay(150);
      printf("Cone has been picked up.");
      }
      if (steps == 1 && CONE_ARM_UP_BUTTON == 1){
      stackCone();
      steps = 2;
      delay(150);
      printf("Cone has been stacked.");
      }
      if (steps == 2 && CONE_ARM_DOWN_BUTTON == 1){
      moveConeGround();
      delay (150);
      printf("Arm has been reset.");
      printf("Number of Cones: %d",numCones);
      steps = 0;
      }

      } */ //end of automated control mode

    // End of while loop
    //taskDelete(autoPilotHandle);
    taskDelete(driverControlHandle);

}
