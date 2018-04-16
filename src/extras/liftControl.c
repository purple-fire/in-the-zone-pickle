/**
 * Functions for controlling the lift, during both driver control and
 * autonomous.
 */
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
    [2]  = {LIFT_DOWN,  LIFT_DOWN,      CONE_UP},
    [3]  = {158,        LIFT_DOWN,      CONE_UP},
    [4]  = {277,        15,             CONE_UP},
    [5]  = {395,        126,            CONE_UP},
    [6]  = {481,        257,            1165},
    [7]  = {685,        380,            1180},
    [8]  = {715,        505,            1180},
    [9]  = {875,        620,            1190},
    [10] = {1020,       760,            1200},
    [11] = {1150,       855,            1220},
    [12] = {1230,       970,            1220},
    [13] = {1400,       1090,           1220},
    [14] = {1505,       1200,           1220},
    [15] = {1630,       1360,           1200},
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
    pidDataInit(&lift, .25, 0, 0, 125, 4095, 150);

    while (true)
    {
        mogoPosition = analogRead(MOGO_POT_PORT);
        if (mogoToggle==1)
        {
            int errorLiftAngle = mogoTarget - mogoPosition;
            int liftPowerOut = pidNextIteration(&mogoLift, errorLiftAngle);
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
            int liftPowerOut = pidNextIteration(&coneLift, errorLiftAngle);
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
            int liftPowerOut = pidNextIteration(&lift, errorLiftAngle);
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
    if (numCones < 3){
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

void stackCone() {
    if (numCones >= CONE_COUNT_MAX || grabState == GRABBED_STACK) {
        return;
    }

    setLiftHeightBlock(coneStackPositions[numCones].liftPosPre);
    setConeAngleBlock( coneStackPositions[numCones].conePos);
    setLiftHeightBlock(coneStackPositions[numCones].liftPosPost);

    grabState = GRABBED_STACK;
    numCones++;
}

void ungrabStack() {
    if (grabState != GRABBED_STACK) {
        return;
    }

    /* Set lift height to the pre-stack height of the previous cone. */
    motorSet(goliathMotor, GOLIATH_OUT);
    setLiftHeightBlock(numCones == 0
            ? liftPosition
            : coneStackPositions[numCones - 1].liftPosPre);
    motorStop(goliathMotor);
    setConeAngleBlock(CONE_HALF);
}

void decrementNumCones(){
    if (numCones > 0) {
        numCones--;
    }
}

void resetCones(){
    numCones = 0;
}

void pickupCone(int mode) {

    if (mode == 0) { //Autonomous
        setConeAngle(CONE_DOWN);
        setLiftHeight(LIFT_DOWN);
        motorSet(goliathMotor,GOLIATH_IN);
        delay(150);
        setConeAngle(CONE_HALF);
        motorSet(goliathMotor,0);
    } else {   //For use in teleop
        setConeAngle(CONE_DOWN);

        /* TODO
         * Don'e have a while loop inside an action in teleop (since this blocks
         * other button input)
         */
        while (CONE_ARM_DOWN_BUTTON == 1){
            motorSet(goliathMotor,GOLIATH_IN);
        }

        setConeAngle(CONE_HALF);
        motorSet(goliathMotor,0);
    }

    grabState = GRABBED_CONE;
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

void setLiftHeightBlock(int angle) {
    setLiftHeight(angle);
    while (ABS(liftPosition - angle) > LIFT_THRESH) {}
}

void setMogoAngleBlock(int angle) {
    setMogoAngle(angle);
    while (ABS(mogoPosition - angle) > MOGO_THRESH) {}
}

void setConeAngleBlock(int angle) {
    setLiftHeight(angle);
    while (ABS(conePosition - angle) > CONE_THRESH) {}
}

