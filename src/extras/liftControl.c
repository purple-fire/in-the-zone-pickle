/**
 * Functions for controlling the lift, during both driver control and
 * autonomous.
 */

#include <limits.h>

#include "main.h"
#include "liftControl.h"
#include "pid.h"
#include "utilities.h"
#include "motor.h"

int stackConeCount = 0;
int stationaryConeCount = 0;

int mogoTarget = MOGO_UP;
int mogoPosition;
int mogoToggle = 1;

int coneTarget = CONE_UP;
int conePosition;
int coneToggle = 1;

int liftTarget = LIFT_DOWN;
int liftPosition;
int liftToggle = 1;



GrabState grabState = GRABBED_NONE;

StackConePos stackConePositions[STACK_CONES_MAX] = {
    [0]  = {LIFT_DOWN,  LIFT_DOWN,      CONE_UP},
    [1]  = {LIFT_DOWN,  LIFT_DOWN,      CONE_UP},
    [2]  = {40,         LIFT_DOWN,      CONE_UP},
    [3]  = {220,        LIFT_DOWN,      CONE_UP},
    [4]  = {380,        95,             CONE_UP},
    [5]  = {520,        205,            CONE_UP},
    [6]  = {610,        370,            1165},
    [7]  = {780,        460,            1180},
    [8]  = {850,        580,            1180},
    [9]  = {980,        700,            1190},
    [10] = {1110,       820,            1200},
    [11] = {1220,       950,            1220},
    [12] = {1350,       1040,           1220},
    [13] = {1480,       1160,           1220},
    [14] = {1670,       1290,           1220},
    [15] = {1820,       1420,           1200},
};

StationaryConePos stationaryConePositions[STATIONARY_CONES_MAX] = {
    [0] = { 790,    2860 },
    [1] = { 840,    2700 },
    [2] = { 940,    2700 },
    [3] = { 1100,   2730 },
    [4] = { 1220,   2670 },
    [5] = { 1350,   2730 },
    [6] = { 1520,   2770 },
    [7] = { 1630,   2700 },
};

void liftControl(void *parameter)
{
    /* TODO Is there any benefit of having all three lifts controlled in 1
     * thread vs 1 thread for eaech component? Is there any benefit of having a
     * separate thread for lift control at all as opposed to setMogoAngle(),
     * setConeAngle(), etc. blocking until the position it met?
     */

    // Separate variables to simplify tuning
    PIDData mogoLift;
    pidDataInit(&mogoLift, 0.18, 0, 0, 125, 4095, 150);

    PIDData coneLift;
    pidDataInit(&coneLift, .1, 0, 0, 125, 4095, 150);

    PIDData lift;
    pidDataInit(&lift, 0.7, 0.000, 2.0, 125, 4095, 200);

    while (true)
    {
        mogoPosition = analogRead(MOGO_POT_PORT);
        liftPosition = analogRead(LIFT_POT_PORT);
        conePosition = analogRead(CONE_POT_PORT);

        if (mogoToggle==1)
        {
            int errorLiftAngle = mogoTarget - mogoPosition;
            int liftPowerOut = motorPowerLimit(
                    pidNextIteration(&mogoLift, errorLiftAngle));
            liftPosition = analogRead(LIFT_POT_PORT);
            if(ABS(errorLiftAngle)>10){
                motorSet(mogoMotor,-liftPowerOut);
            }
            else{
                motorSet(mogoMotor,0);
            }
        }
        else
        {
            motorStop(mogoMotor);
        }

        conePosition = analogRead(CONE_POT_PORT);
        if (coneToggle==1)
        {
            int errorLiftAngle = coneTarget - conePosition;
            int liftPowerOut = motorPowerLimit(
                    pidNextIteration(&coneLift, errorLiftAngle));
            motorSet(coneLiftMotor,liftPowerOut);
        }
        else
        {
            motorStop(coneLiftMotor);
        }

        liftPosition = analogRead(LIFT_POT_PORT);
        if (liftToggle==1)
        {
            int errorLiftAngle = liftTarget - liftPosition;
            int liftPowerOut = motorPowerLimit(
                    pidNextIteration(&lift, errorLiftAngle));
            motorSet(liftMotor,-liftPowerOut);
            motorSet(liftMotorAux,-liftPowerOut);
        }
        /*
           else if (liftToggle==2)
           {
           motorStop(liftMotor);
           motorStop(liftMotorAux);
           }
           */

        delay(20);
    }
}

bool stackCone() {
    if (stackConeCount >= STACK_CONES_MAX) {
        return false;
    }

    int liftPosPre  = stackConePositions[stackConeCount].liftPosPre;
    int liftPosPost = stackConePositions[stackConeCount].liftPosPost;
    int conePos     = stackConePositions[stackConeCount].conePos;

    if (!setLiftHeightBlock(liftPosPre, 2000))  { /* return false; */ }
    if (!setConeAngleBlock(conePos, 1000))      { /* return false; */ }
    if (!setLiftHeightBlock(liftPosPost, 2000)) { /* return false; */ }

    grabState = GRABBED_STACK;
    incStackCones();
    return true;
}

bool stackConeLoader(){
  if (stackConeCount >= STACK_CONES_MAX) {
      return false;
  }

  int liftPosPre  = stackConePositions[stackConeCount].liftPosPre;
  int liftPosPost = stackConePositions[stackConeCount].liftPosPost;
  int conePos     = stackConePositions[stackConeCount].conePos;

  if (stackConeCount < 4){
      if (!setLiftHeightBlock(LIFT_LOADER, 500))  { return false; }
  }
  else{
      if (!setLiftHeightBlock(liftPosPre, 2000))  { return false; }
  }

  if (!setConeAngleBlock(conePos, 1000))      { return false; }
  if (!setLiftHeightBlock(liftPosPost, 2000)) { return false; }
  grabState = GRABBED_STACK;
  incStackCones();
  return true;

}

bool stackConeStationary() {
    if (stationaryConeCount >= STATIONARY_CONES_MAX || stackConeCount < 5) {
        return false;
    }


    int liftPosStat = stationaryConePositions[stationaryConeCount].liftPos;
    int conePos = stationaryConePositions[stationaryConeCount].conePos;

    liftToggle = 0;
    motorSet(liftMotor,-127);
    motorSet(liftMotorAux,-127);
    motorSet(goliathMotor, GOLIATH_IN - 40);
    delay(300);
    motorSet(liftMotor,0);
    motorSet(liftMotor,0);
    motorSet(goliathMotor,0);
    liftToggle = 1;
    setConeAngle(conePos);
    if (!setLiftHeightBlock(liftPosStat, 2000))  { return false; }
    motorSet(goliathMotor, GOLIATH_OUT);
    delay(400);
    incStationaryCones();
    decStackCones();
    motorSet(goliathMotor,0);
    return true;
}

bool grabStack(int mode) {
    /* With fewer than 5 cones the cone lift doesn't touch the top of the stack.
     */
    if (stackConeCount < 5) {
        /* TODO Should we return true if GRABBED_CONE since the end result of
         * this is to grab a cone?
         */
        return false;
    }

    int liftPosPre  = stackConePositions[stackConeCount-1].liftPosPre;
    int conePos     = stackConePositions[stackConeCount-1].conePos;
    int liftPosPost = stackConePositions[stackConeCount-1].liftPosPost;

    if (!setLiftHeightBlock(liftPosPre, 2000))   { return false; }
    if (!setConeAngleBlock(conePos, 1000))       { return false; }
    motorSet(goliathMotor, GOLIATH_IN);
    if (!setLiftHeightBlock(liftPosPost, 1000))  {
        motorStop(goliathMotor);
        return false;
    }
    motorStop(goliathMotor);

    grabState = GRABBED_STACK;
    return true;
}

void dropCone() {
    motorSet(liftMotor, 60);
    motorSet(liftMotorAux, 60);
    delay(100);
    motorSet(goliathMotor, GOLIATH_OUT);
    delay(200);
    motorStop(goliathMotor);
    motorStop(liftMotor);
    motorStop(liftMotorAux);
}

bool ungrabStack() {
    motorSet(goliathMotor, GOLIATH_OUT);
    /* After the 3rd cone the lift needs to move up and the goliath needs to
     * outtake to realse the cone.
     */
    if (stackConeCount > 3) {
        /* Pulse the lifit up to clear the cone */
        int toggleSave = liftToggle;
        liftToggle = 0;

        motorSet(liftMotor, -127);
        motorSet(liftMotorAux, -127);
        if (liftPosition < 800) {
            delay(250);
        } else {
            delay(400);
        }
        motorStop(liftMotor);
        motorStop(liftMotorAux);

        liftToggle = toggleSave;
    } else {
        /* Give the goliath some time to outtake if the lift is not moved */
        delay(200);
    }

    motorStop(goliathMotor);

    grabState = GRABBED_NONE;
    return true;
}

bool ungrabStationary() {
    if (grabState != GRABBED_STATIONARY) {
        return false;
    }

    motorSet(goliathMotor, GOLIATH_OUT);
    /* Move up to liftPosPre of the previous cone */
    setLiftHeightBlock(
            stationaryConePositions[stationaryConeCount].liftPos,
            1000);

    motorStop(goliathMotor);
    setConeAngleBlock(CONE_HALF, 1000);

    grabState = GRABBED_NONE;
    return true;
}

void incStackCones() {
    if (stackConeCount < STACK_CONES_MAX) {
        stackConeCount++;
        ledSendConeCount(stackConeCount);
    }
}

void decStackCones() {
    if (stackConeCount > 0) {
        stackConeCount--;
        ledSendConeCount(stackConeCount);
    }
}

void resetStackCones(){
    stackConeCount = 0;
    ledSendConeCount(stackConeCount);
}

void incStationaryCones() {
    if (stationaryConeCount < STATIONARY_CONES_MAX) {
        stationaryConeCount++;
    }
}

void decStationaryCones() {
    if (stationaryConeCount > 0) {
        stationaryConeCount--;
    }
}

void resetStationaryCones(){
    stationaryConeCount = 0;
}

bool pickupCone(int mode) {
    if (mode == 0) { //Autonomous
        motorSet(goliathMotor,GOLIATH_IN);
        setConeAngle(CONE_DOWN); /* Let setLiftHeightBlock() give enough time */
        setLiftHeightBlock(LIFT_DOWN, 2000);
        liftToggle = 0;
        motorSet(liftMotor, 80);
        motorSet(liftMotorAux, 80);
        delay(800);
        motorStop(liftMotor);
        motorStop(liftMotorAux);
        motorStop(goliathMotor);
    } else {   //For use in teleop
        int toggleSave = liftToggle;
        liftToggle = 1;

        /* Let the operator decide when to stop these */
        setLiftHeight(LIFT_DOWN);
        setConeAngle(CONE_DOWN);

        /* TODO
         * Don't have a while loop inside an action in teleop (since this blocks
         * other button input)
         */
        while (CONE_ARM_DOWN_BUTTON == 1){
            motorSet(goliathMotor,GOLIATH_IN);
        }

        motorStop(goliathMotor);
        setConeAngleBlock(CONE_HALF, 100); /* Don't do anything if this fails */
        liftToggle = toggleSave;
    }

    grabState = GRABBED_CONE;
    return true;
}

bool pickupConeLoader(int mode) {
    if (mode == 0) { //Autonomous
        motorSet(goliathMotor,GOLIATH_IN);
        if (liftPosition < LIFT_LOADER) {
            setLiftHeightBlock(LIFT_LOADER, 500);
            setConeAngleBlock(CONE_DOWN, 500);
        } else {
            setConeAngleBlock(CONE_DOWN, 500);
            setLiftHeightBlock(LIFT_LOADER, 500);
        }
        delay(500);
        motorStop(goliathMotor);
    } else {   //For use in teleop
        int toggleSave = liftToggle;
        liftToggle = 1;

        /* Let the operator decide when to stop this */
        if (liftPosition < LIFT_LOADER) {
            setLiftHeightBlock(LIFT_LOADER, 1000);
        } else {
            setLiftHeight(LIFT_LOADER);
        }

        setConeAngle(CONE_DOWN);

        /* TODO
         * Don't have a while loop inside an action in teleop (since this blocks
         * other button input)
         */
        while (CONE_ARM_DOWN_BUTTON == 1){
            motorSet(goliathMotor,GOLIATH_IN);
        }

        motorStop(goliathMotor);
        liftToggle = toggleSave;
    }

    grabState = GRABBED_CONE;
    return true;
}

void liftClearMogo() {
    int toggleSave = liftToggle;
    liftToggle = 0;

    if (liftPosition < LIFT_MID) {
        if (liftTarget < LIFT_MID) {
            liftTarget = LIFT_MID;
        }

        motorSet(liftMotor, -127);
        motorSet(liftMotorAux, -127);
        delay(150);
        motorStop(liftMotor);
        motorStop(liftMotorAux);
    }

    liftToggle = toggleSave;
}

void setMogoAngle(int liftAngle)
{
    liftClearMogo();
    mogoTarget = liftAngle;
}

void setConeAngle(int liftAngle)
{
    coneTarget = liftAngle;
}

int getConeAngle()
{
    return coneTarget;
}

void setLiftHeight(int liftAngle)
{
    liftTarget = liftAngle;
}

bool setLiftHeightBlock(int angle, int timeout) {
    int toggleSave = liftToggle;
    liftToggle = 1;

    int start = millis();
    if (timeout < 0) {
        timeout = INT_MAX;
    }

    setLiftHeight(angle);
    while (ABS(liftPosition - angle) > LIFT_THRESH) {
        delay(20);

        if (millis() - start > timeout) {
            liftToggle = toggleSave;
            return false;
        }
    }

    liftToggle = toggleSave;
    return true;
}

bool setMogoAngleBlock(int angle, int timeout) {
    int start = millis();
    if (timeout < 0) {
        timeout = INT_MAX;
    }

    setMogoAngle(angle);
    while (ABS(mogoPosition - angle) > MOGO_THRESH) {
        delay(20);

        if (millis() - start > timeout) {
            return false;
        }
    }

    return true;
}

bool setConeAngleBlock(int angle, int timeout) {
    int start = millis();
    if (timeout < 0) {
        timeout = INT_MAX;
    }

    setConeAngle(angle);
    while (ABS(conePosition - angle) > CONE_THRESH) {
        delay(20);

        if (millis() - start > timeout) {
            return false;
        }
    }

    return true;
}
