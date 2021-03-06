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
int stackingMode = 0;
int leftDist;
int rightDist;
/**
 * Driver control with tank-style controls.
 */
static void driverControl(void *parameter) {

    chassisStop();

    while (true) {
      leftDist = ultrasonicGet(leftSonar);
      rightDist = ultrasonicGet(rightSonar);
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

void changeStackingMode(int mode){
  /*
  Mode and Description

  0: Picking up from the GROUND
  1: Picking up from the AUTOLOADER
  2: Stacking from current stack to STATIONARY
  3: Stacking from current stack to another MOGO
  */
  stackingMode = mode;
  char sendingChar = 'u' + mode;
  fputc(sendingChar, uart1);
}
static void startAutoPilot(void *parameter) {
   chassisStopSmooth();
   autonomous();
}

void operatorControl() {
    TaskHandle autoPilotHandle, driverControlHandle;
    driveMode = DRIVE_ARCADE;
    driverControlHandle = taskCreate(driverControl, TASK_DEFAULT_STACK_SIZE, NULL,
            TASK_PRIORITY_DEFAULT);

    setMogoAngle(analogRead(MOGO_POT_PORT));
    mogoToggle = 1;
    setConeAngle(analogRead(CONE_POT_PORT));

    int toggleCone = 0;//,toggleLift,toggleMogo;

    bool coneArmUpPressed = false,
         coneArmDownPressed = false,
         liftUpPressed = false,
         coneDecPressed = false,
         coneIncPressed = false,
         changeStackPressed = false;

    while (true) {
        //motorSet(5,-127); //SET PIN 5 HIGH



        if (LIFT_CONTROL_SWITCH == 1){
            if (liftMode == 0){

                liftMode = 1;
                steps = 0;
                resetStackCones();
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
            autoPilotHandle = taskCreate(startAutoPilot, TASK_DEFAULT_STACK_SIZE,
                                         NULL, TASK_PRIORITY_DEFAULT);
            driveMode = DRIVE_AUTO;
        }else if (MODE_SWITCH_RIGHT_BUTTON == 1) {
            if (driveMode == DRIVE_AUTO) {
                taskDelete(autoPilotHandle);
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




            } else if (MOGO_DROP_LOW_BUTTON == 1) {
                setMogoAngle(MOGO_DROP_LOW);
                if (stackConeCount > 5){
                  int liftPosPre  = stackConePositions[stackConeCount].liftPosPre;
                  liftToggle = 0;
                  setConeAngleBlock(CONE_UP_OFFSET, 700);

                  motorSet(goliathMotor, GOLIATH_OUT);
                  motorSet(liftMotor, -127);
                  motorSet(liftMotorAux, -127);

                  delay(600);
                  motorSet(liftMotor, 0);
                  motorSet(liftMotorAux, 0);
                  motorSet(goliathMotor, 0);
                }
                resetStackCones();

                char rainbow = 'R';
                fputc(rainbow,uart1);
            }
            else if (MOGO_DROP_HIGH_BUTTON == 1) {
                setMogoAngle(MOGO_DROP_HIGH);
                resetStackCones();
                char rainbow = 'R';
                fputc(rainbow,uart1);
            }
            //Automated controls for the cone arm

            if (!CONE_DEC_BUTTON) {
                coneDecPressed = false;
            } else if (!coneDecPressed) {
                decStackCones();
                coneDecPressed = true;
            }

            if (!CONE_INC_BUTTON) {
                coneIncPressed = false;
            } else if (!coneIncPressed) {
                incStackCones();
                coneIncPressed = true;
            }


            if (!STACKING_MODE_CHANGE_BUTTON){
              changeStackPressed = false;
            } else if (!changeStackPressed){

              changeStackingMode(stackingMode);
              stackingMode++;
              changeStackPressed = true;
              if (stackingMode == 4){
                stackingMode = 0;
              }


            }

            //Goliath Controls
            if (GOLIATH_IN_BUTTON) {
                motorSet(goliathMotor,GOLIATH_IN);
            } else if (GOLIATH_OUT_BUTTON) {
                motorSet(goliathMotor,GOLIATH_OUT);
            } else {
                motorStop(goliathMotor);
            }

            //Toggle Cone states:
            //Top:  1
            //Mid:  0
            //Btm: -1

            //MANUAL LIFT CONTROL

            if (liftMode == 0){
                /* This if-else setup if used to only perform button actions when a
                 * button is initially pressed, not when it is held down for
                 * multiple iterations of the input loop.
                 */
                if (!CONE_ARM_UP_BUTTON) {
                    coneArmUpPressed = false;
                } else if (!coneArmUpPressed) {
                    coneArmUpPressed = true;


                    if (toggleCone == 0) {
                        //Cone arm is at the bottom. Press RIGHT to go to middle.
                        setConeAngle(CONE_HALF);
                        toggleCone = 1;
                    } else {
                        //Cone arm is at the middle. press RIGHT to go to up.
                        setConeAngle(CONE_UP);
                        toggleCone = 0;
                    }
                }


                if (!CONE_ARM_DOWN_BUTTON) {
                    coneArmDownPressed = false;
                } else if (!coneArmDownPressed) {
                    coneArmDownPressed = true;

                        liftToggle = 1;


                switch (stackingMode){

                case 0: //NORMAL STACKING FROM THE GROUND
                        switch (grabState) {
                        case GRABBED_STACK:
                            /* Need to move the intake to clear the stack first */
                            ungrabStack();
                            pickupCone(1);
                            break;
                        case GRABBED_CONE:
                        case GRABBED_NONE:
                            pickupCone(1);
                            break;
                        case GRABBED_STATIONARY:
                            /* TODO What to do in this case? */
                            break;
                        }

                        break;

               case 1: //Stacking from the autoloader
                     switch (grabState) {
                     case GRABBED_STACK:
                         /* Need to move the intake to clear the stack first */
                         ungrabStack();
                         pickupConeLoader(1);
                         break;
                     case GRABBED_CONE:
                     case GRABBED_NONE:
                         pickupConeLoader(1);
                         break;
                     case GRABBED_STATIONARY:
                         /* TODO What to do in this case? */
                         break;
                     }

               break;

               case 2: //Transfer from the stack to the stationary goal
               switch (grabState) {
               case GRABBED_STACK:
               case GRABBED_CONE:
                   /* Need to move the intake to clear the stack first */
                   stackConeStationary();
                   break;

               case GRABBED_NONE:

                   break;
               case GRABBED_STATIONARY:
                   /* TODO What to do in this case? */
                   break;
               }
               break;

               case 3:  //Transfer from the stack to another mogo

               break;

                      }
                }

                //RD4B Controls
                //Semi-automated stacking where manual control over the lift is
                //given, but upon release it autostacks
                if (!LIFT_UP_BUTTON) {
                    liftUpPressed = false;
                } else if (!liftUpPressed) {

                    liftUpPressed = true;
                    liftToggle = 1;

                    switch (stackingMode){
                    case 0:
                        stackCone();
                        break;

                    case 1:
                        stackConeLoader();
                        break;

                    case 2:
                        grabStack(0);
                        break;

                    case 3:

                        break;
                    default:
                        stackCone();
                    }
                }

                /* Only do manual ones if the automated one is not being used */
                if (MANUAL_LIFT_UP_BUTTON == 1){
                    liftToggle = 0;
                    motorSet(liftMotor,-127);
                    motorSet(liftMotorAux,-127);
                } else if (LIFT_DOWN_BUTTON == 1) {
                    liftToggle = 0;
                    motorSet(liftMotor,127);
                    motorSet(liftMotorAux,127);
                } else if (!liftUpPressed) {
                    /* If nothing is pressed, let it hang */
                    liftToggle = 0;
                    motorStop(liftMotor);
                    motorStop(liftMotorAux);
                }
            }

            delay(20);
        } ///END AUTO


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
    taskDelete(autoPilotHandle);
    taskDelete(driverControlHandle);

}
