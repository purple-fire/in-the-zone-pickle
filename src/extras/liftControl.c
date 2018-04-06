/**
 * Functions for controlling the lift, during both driver control and
 * autonomous.
 */

#include "liftControl.h"
#include "pid.h"
#include "utilities.h"
#include "motor.h"
//oof
int mogoTarget = MOGO_UP;
int mogoPosition;
int mogoToggle =1;

int coneTarget = MOGO_UP;
int conePosition;
int coneToggle =1;

int liftTarget = LIFT_DOWN;
int liftPosition;
int liftToggle =2;

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
            printf("coneError: %d\n", errorLiftAngle);
            motorSet(coneLiftMotor,liftPowerOut);
        }
        else
        {
            motorStop(coneLiftMotor);
        }

        //printf("liftPosition: %d\n", liftPosition);
        //printf("liftPower: %d\n", liftPowerOut);

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
