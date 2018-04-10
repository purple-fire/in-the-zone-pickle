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
int mogoToggle =1;

int coneTarget = MOGO_UP;
int conePosition;
int coneToggle =1;

int liftTarget = LIFT_DOWN;
int liftPosition;
int liftToggle =0;

void liftControl(void *parameter)
{

    // Separate variables to simplify tuning
    PIDData mogoLift;
    pidDataInit(&mogoLift, 0.18, 0, 0, 125, 4095, 150);

    PIDData coneLift;
    pidDataInit(&coneLift, .1, 0, 0, 125, 4095, 150);

    PIDData lift;
    pidDataInit(&lift, .25, 0, 0, 125, 4095, 150);


    while (true)
    {
        if (mogoToggle==1)
        {
            mogoPosition = analogRead(MOGO_POT_PORT);
            int errorLiftAngle = mogoTarget - mogoPosition;
            int liftPowerOut = pidNextIteration(&mogoLift, errorLiftAngle);
            liftPosition = analogRead(LIFT_POT_PORT);
            //printf("mogoPosition: %d\n", mogoPosition);
            //printf("mogoError: %d\n", errorLiftAngle);
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

        if (coneToggle==1)
        {
            conePosition = analogRead(CONE_POT_PORT);
            int errorLiftAngle = coneTarget - conePosition;
            int liftPowerOut = pidNextIteration(&coneLift, errorLiftAngle);
            printf("conePosition: %d\n", conePosition);
            //printf("coneError: %d\n", errorLiftAngle);
            motorSet(coneLiftMotor,liftPowerOut);
        }
        else
        {
            motorStop(coneLiftMotor);
        }

/*
        if (liftToggle==1)
        {
            liftPosition = analogRead(LIFT_POT_PORT);
            int errorLiftAngle = liftTarget - liftPosition;
            int liftPowerOut = pidNextIteration(&lift, errorLiftAngle);
            //printf("liftPosition: %d\n", liftPosition);
            //printf("liftPower: %d\n", liftPowerOut);
            motorSet(liftMotor,-liftPowerOut);
            motorSet(liftMotorAux,-liftPowerOut);
        }
        else if (liftToggle!=2)
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
void stackCone(){
  numCones++;
  backupCones = numCones;

  if (numCones < 3){
    setLiftHeight(LIFT_DOWN);
  }
  else{
    setLiftHeight(numCones*LIFT_INC_HALF + 150);
    if (numCones <= 10){
    setConeAngle(CONE_UP);
  }
    else{
    setConeAngle (CONE_UP_OFFSET);
    }
    setLiftHeight(numCones*LIFT_INC_HALF);


  }

}

void decrementCone(){
  numCones--;

}

void resetCones(){
  numCones = 0;
}

void reverseConeState(){
    numCones = backupCones;
}
void pickupCone(int mode){

  if (mode == 0){ //Autonomous
    setConeAngle(CONE_DOWN);
    motorSet(goliathMotor,GOLIATH_IN);
    delay(150);
    setConeAngle(CONE_HALF);
    motorSet(goliathMotor,0);
  }
  else{   //For use in teleop
    setConeAngle(CONE_DOWN);

    while (CONE_ARM_DOWN_BUTTON == 1){
      motorSet(goliathMotor,GOLIATH_IN);
    }
    setConeAngle(CONE_HALF);
    motorSet(goliathMotor,0);
  }
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
