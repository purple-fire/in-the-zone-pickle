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
    [2]  = {130,        LIFT_DOWN,      CONE_UP},
    [3]  = {250,        LIFT_DOWN,      CONE_UP},
    [4]  = {375,        30,             CONE_UP},
    [5]  = {475,        165,            CONE_UP},
    [6]  = {575,        257,            1165},
    [7]  = {685,        420,            1180},
    [8]  = {785,        525,            1180},
    [9]  = {895,        655,            1190},
    [10] = {1000,       760,            1200},
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
    pidDataInit(&lift, .3, 0, 0, 125, 4095, 150);

    while (true)
    {
        int liftPos = analogRead(LIFT_POT_PORT);
        printf("Lift Height: %d\n",liftPos);
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

void stackCone() {
    if (numCones >= CONE_COUNT_MAX || grabState == GRABBED_STACK) {
        return;
    }

    int liftPosPre  = coneStackPositions[numCones].liftPosPre;
    int liftPosPost = coneStackPositions[numCones].liftPosPost;
    int conePos     = coneStackPositions[numCones].conePos;

    setLiftHeight(liftPosPre);

    if (numCones > 1){
        delay((int)(liftPosPre)*0.5);
    }
    setConeAngle(conePos);

    delay(175);

    setLiftHeight(liftPosPost - 100);

    grabState = GRABBED_STACK;
    numCones++;
}

void ungrabStack() {

    if (grabState != GRABBED_STACK) {
        return;
    }

    /* Set lift height to the pre-stack height of the previous cone. */
    motorSet(goliathMotor, GOLIATH_OUT);
    int currentLift = analogRead(LIFT_POT_PORT);
    if (numCones >= 3){
        setLiftHeight(currentLift+300);
        delay((currentLift + 300) / 2);
    }
    motorStop(goliathMotor);
    setConeAngle(CONE_HALF);
    setLiftHeight(LIFT_DOWN);
}

void decrementNumCones(){
    if (numCones > 0) {
        numCones--;
        delay(200);
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
        delay(100);
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
    setConeAngle(angle);
    while (ABS(conePosition - angle) > CONE_THRESH) {}
}
