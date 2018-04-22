#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void baseControl(float target, float power, float integralRange, float timeOut)
{

    // Do PID on each side separately to help account for physical inaccuracies
    PIDData rightData;
    PIDData leftData;

    float kp = 0.2;
    float ki = 0.0005;
    float kd = 0.0;
    float maxPower = power;

    pidDataInit(&leftData, kp, ki, kd, maxPower, 32767, integralRange);
    pidDataInit(&rightData, kp, ki, kd, maxPower, 32767, integralRange);

    encoderReset(BLEncoder);
    encoderReset(BREncoder);

    target = INCHES_TO_TICKS(target);

    long T1, T2;
    T1 = millis();
    T2 = millis();

    timeOut = timeOut*1000;

    while ((T1 > (millis() - 200))&&(T2 > (millis() - timeOut))) {

        rightEncoderValue = encoderGet(BREncoder);
        leftEncoderValue = encoderGet(BLEncoder);

        rightError = target - rightEncoderValue;

        rightPower =  motorPowerLimit(pidNextIteration(&rightData, rightError));


        leftError = target - leftEncoderValue;
        leftPower =  motorPowerLimit(pidNextIteration(&leftData, leftError));

        rightMotorsSet(capMotorPower(rightPower,maxPower));
        leftMotorsSet(capMotorPower(leftPower,maxPower));

        if((ABS(leftError)>60)||(ABS(rightError)>60)){
            T1 = millis();
        }

        delay(20);
    }

    chassisStop();
}

void baseTurn(float target, bool leftToggle, bool rightToggle, float timeOut)
{
    const float kp = 2.30;
    const float ki = 0.075;
    const float kd = 1.0;

    /* const float kp = 0.5; */
    /* const float ki = 0.0; */
    /* const float kd = 0.0; */

    PIDData data;
    pidDataInit(&data, kp, ki, kd, 10, 360, 10);

    int oldValue = INT_MAX;
    gyroValue = devgyroGet(&gyroDev);
    while (oldValue != gyroValue) {
        delay(50);
        oldValue = gyroValue;
        gyroValue = devgyroGet(&gyroDev);
    }

    long T1, T2;
    T1 = millis();
    T2 = millis();

    timeOut = timeOut*1000;

    while ((T1 > (millis() - 200))&&(T2 > (millis() - timeOut))) {
        gyroValue = devgyroGet(&gyroDev);
        turnError = target - gyroValue;
        turnPower = motorPowerLimit(pidNextIteration(&data, turnError));

        // TODO Should we check that each side moves the same amount and adjust
        // them afterwards if not?
        if(rightToggle){
            rightMotorsSet(turnPower);
        } else {
            rightMotorsSet(0);
        }

        if(leftToggle){
            leftMotorsSet(-turnPower);
        } else {
            leftMotorsSet(0);
        }

        if(ABS(turnError)>3){
            T1 = millis();
        }

        delay(20);
    }

    chassisStop ();
}

/**
 * Time-based drive function for when we don't care too much about distance
 * (when going over the bump and aligning along walls/pipes).
 */
void driveTime(float powerL, float powerR, bool coast, float timeOut)
{
    long T1;
    T1 = millis();

    timeOut = timeOut*1000;

    while (T1 > (millis() - timeOut)) {
        rightMotorsSet(powerR);
        leftMotorsSet(powerL);
    }

    if (coast) {
        rightMotorsSet(0);
        leftMotorsSet(0);
    } else {
        rightMotorsBrake();
        leftMotorsBrake();
    }
}

/**
 * Drives until it senses a obstacle
 */

void wallBump(int threshold, float power, float timeOut, int angle)
{
    long T1;
    T1 = millis();
    int distance = ULTRA_BAD_RESPONSE;
    bool bumped = true;
    timeOut = timeOut*1000;

    while (distance > threshold || distance == ULTRA_BAD_RESPONSE) {
      distance = 0; /* TODO Put a sensor on or remove wallBump() altogether */
      rightMotorsSet(power);
      leftMotorsSet(power);

      if (T1 < millis() - timeOut) {
          bumped = false;
          break;
      }
    }

    chassisStop();

    if (bumped) {
        /* Only reset the gyro if we actually bump (as opposed to timing out) */
        devgyroResetTo(&gyroDev, angle);
    }

    delay(100);
}

void loaderAlign(int power, int timeout) {
    long startTime = millis();

    int dist = ULTRA_BAD_RESPONSE;
    while (dist == ULTRA_BAD_RESPONSE || dist > LOADER_DISTANCE + 8) {
        rightMotorsSet(power);
        leftMotorsSet(power);

        if (millis() - startTime > timeout) {
            break;
        }

        delay(20);
        dist = ultrasonicGet(leftSonar);
    }

    chassisStop();
}

