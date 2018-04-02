/**
 * Functions for controlling the lift, during both driver control and
 * autonomous.
 */

#include "liftControl.h"
#include "pid.h"
#include "utilities.h"
#include "motor.h"

int mogoTarget = MOGO_UP;
int mogoPosition;
int mogoToggle =1;

int coneTarget = MOGO_UP;
int conePosition;
int coneToggle =1;

int liftTarget = LIFT_DOWN;
int liftPosition;
int liftToggle =1;

void liftControl(void *parameter)
{

    // Separate variables to simplify tuning
    PIDData mogoLift;
    pidDataInit(&mogoLift, 0.18, 0, 0, 125, 4095, 150);

    PIDData coneLift;
    pidDataInit(&coneLift, .18, 0, 0, 125, 4095, 150);

    PIDData lift;
    pidDataInit(&lift, .25, 0, 0, 125, 4095, 150);

    while (true)
    {

        if (mogoToggle==1)
        {
            mogoPosition = analogRead(MOGO_POT_PORT);
            int errorLiftAngle = mogoTarget - mogoPosition;
            int liftPowerOut = pidNextIteration(&mogoLift, errorLiftAngle);
            //printf("mogoPosition: %d\n", mogoPosition);
            //printf("liftPower: %d\n", liftPowerOut);
            motorSet(mogoMotor,-liftPowerOut);
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
            //printf("conePosition: %d\n", conePosition);
            //printf("liftPower: %d\n", liftPowerOut);
            motorSet(coneLiftMotor,liftPowerOut);
        }
        else
        {
            motorStop(coneLiftMotor);
        }

        if (liftToggle==1)
        {
            liftPosition = analogRead(LIFT_POT_PORT);
            int errorLiftAngle = liftTarget - liftPosition;
            int liftPowerOut = pidNextIteration(&lift, errorLiftAngle);
            printf("liftPosition: %d\n", liftPosition);
            printf("liftPower: %d\n", liftPowerOut);
            motorSet(liftMotor,-liftPowerOut);
        }
        else if (liftToggle!=2)
        {
            motorStop(liftMotor);
        }

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

void setLiftHeight(int liftAngle)
{
    liftTarget = liftAngle;
}

void setLiftState(int state)
{
    liftToggle = state;
}

int getLiftState()
{
    return liftToggle;
}
