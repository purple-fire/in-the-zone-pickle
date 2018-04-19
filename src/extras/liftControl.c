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

int numCones = 0;
int backupCones = 0;
//oof
int mogoTarget = MOGO_UP;
int mogoPosition;
int mogoToggle = 1;

int coneTarget = MOGO_UP;
int conePosition;
int coneToggle = 1;

int liftTarget = LIFT_DOWN;
int liftPosition;
int liftToggle = 1;

GrabState grabState = GRABBED_NONE;

ConeStackPos coneStackPositions[CONE_COUNT_MAX] = {
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
    pidDataInit(&lift, 1.0, 0.0, 0.1, 125, 4095, 40);

    while (true)
    {
        mogoPosition = analogRead(MOGO_POT_PORT);
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

void moveConeGround(){
    if (numCones < 2){
        setConeAngle(CONE_HALF);
        setLiftHeight(LIFT_DOWN);
    }
    else{
        motorSet(goliathMotor,GOLIATH_OUT);
        delay(150);
        setLiftHeight(numCones*LIFT_INC_HALF + 150);
        motorSet(goliathMotor,0);
        setConeAngle(CONE_HALF);
        setLiftHeight(LIFT_DOWN);
    }
}

void moveConeLoader(){

}

bool stackCone() {
    if (numCones >= CONE_COUNT_MAX || grabState == GRABBED_STACK) {
        return false;
    }

    int liftPosPre  = coneStackPositions[numCones].liftPosPre;
    int liftPosPost = coneStackPositions[numCones].liftPosPost;
    int conePos     = coneStackPositions[numCones].conePos;

    if (!setLiftHeightBlock(liftPosPre, 250))   { return false; }
    if (!setConeAngleBlock(conePos, 100))       { return false; }
    if (!setLiftHeightBlock(liftPosPost, 100))  { return false; }

    grabState = GRABBED_STACK;
    numCones++;
    return true;
}

bool ungrabStack() {
    if (grabState != GRABBED_STACK) {
        return false;
    }

    motorSet(goliathMotor, GOLIATH_OUT);
    /* After the 3rd cone the lift needs to move up and the goliath needs to
     * outtake to realse the cone.
     */
    if (numCones > 3) {
        /* Move up to liftPosPre of the previous cone */
        if (!setLiftHeightBlock(
                    coneStackPositions[numCones - 1].liftPosPre, 100)) {
            motorStop(goliathMotor);
            return false;
        }
    } else {
        delay(100);
    }

    motorStop(goliathMotor);
    setConeAngleBlock(CONE_HALF, 100);

    grabState = GRABBED_NONE;
    return true;
}

void incrementNumCones() {
    if (numCones < CONE_COUNT_MAX) {
        numCones++;
    }
}

void decrementNumCones() {
    if (numCones > 0) {
        numCones--;
    }
}

void resetCones(){
    numCones = 0;
}

bool pickupCone(int mode) {

    if (mode == 0) { //Autonomous
        setConeAngle(CONE_DOWN); /* Let setLiftHeightBlock() give enough time */
        setLiftHeightBlock(LIFT_DOWN, 200);
        motorSet(goliathMotor,GOLIATH_IN);
        setConeAngleBlock(CONE_HALF, 100);
        motorStop(goliathMotor);
    } else {   //For use in teleop
        setConeAngle(CONE_DOWN); /* Let the operator decide when to stop */

        /* TODO
         * Don't have a while loop inside an action in teleop (since this blocks
         * other button input)
         */
        while (CONE_ARM_DOWN_BUTTON == 1){
            motorSet(goliathMotor,GOLIATH_IN);
        }

        motorStop(goliathMotor);
        setConeAngleBlock(CONE_HALF, 100); /* Don't do anything if this fails */
    }

    grabState = GRABBED_CONE;
    return true;
}

void setMogoAngle(int liftAngle)
{
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
    int start = millis();
    if (timeout < 0) {
        timeout = INT_MAX;
    }

    setLiftHeight(angle);
    while (ABS(liftPosition - angle) > LIFT_THRESH) {
        delay(20);

        if (millis() - start > timeout) {
            return false;
        }
    }

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

