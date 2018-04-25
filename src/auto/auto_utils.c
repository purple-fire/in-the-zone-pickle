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

void baseTurn(float target, float leftScale, float rightScale, float timeOut)
{
    /* Angles are based off the blue side setup and inverted when on the red
     * side
     */
    if (teamColor == RED_TEAM) {
        target *= -1;
    }

    const float kp = 2.10;
    const float ki = 0.08;
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
        rightMotorsSet(turnPower * rightScale);
        leftMotorsSet(-turnPower * leftScale);

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

void loaderAlign(int power, float timeout) {
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

void barBump(int power, float timeout) {
    long T1, T2;
    T1 = T2 = millis();

    encoderReset(BLEncoder);
    encoderReset(BREncoder);

    int lastLeft  = encoderGet(BLEncoder);
    int lastRight = encoderGet(BREncoder);

    timeout *= 1000;
    while (millis() - T1 < 200 && millis() - T2 < timeout) {

        leftMotorsSet(power);
        rightMotorsSet(power);

        leftEncoderValue =  encoderGet(BLEncoder);
        rightEncoderValue = encoderGet(BREncoder);

        if (ABS(leftEncoderValue - lastLeft) > 2
                || ABS(rightEncoderValue - lastRight) > 2) {
            T1 = millis();
        }

        lastLeft = leftEncoderValue;
        lastRight = rightEncoderValue;

        delay(20);
    }

    chassisStop();
}

void driveSonar(
        int leftTarget, int rightTarget, int power, float integralRange, float timeOut){
    PIDData rightData;
    PIDData leftData;

    float kp = 1.0;
    float ki = 0.000;
    float kd = 0.0;
    float maxPower = power;

    pidDataInit(&leftData, kp, ki, kd, maxPower, 32767, integralRange);
    pidDataInit(&rightData, kp, ki, kd, maxPower, 32767, integralRange);

    int leftDist;
    int rightDist;

    long T1, T2;
    T1 = millis();
    T2 = millis();

    timeOut = timeOut*1000;

    while ((T1 > (millis() - 200))&&(T2 > (millis() - timeOut))) {

        leftDist = ultrasonicGet(leftSonar);
        rightDist = ultrasonicGet(rightSonar);

        if (leftDist != ULTRA_BAD_RESPONSE && leftTarget != ULTRA_BAD_RESPONSE) {
            leftError = leftDist - leftTarget;
            leftPower = capMotorPower(
                    pidNextIteration(&leftData, leftError), maxPower);
            leftMotorsSet(leftPower);
        }

        if (rightDist != ULTRA_BAD_RESPONSE && rightTarget != ULTRA_BAD_RESPONSE) {
            rightError = rightDist - rightTarget;
            rightPower = capMotorPower(
                    pidNextIteration(&rightData, rightError), maxPower);
            rightMotorsSet(rightPower);
        }

        if((ABS(leftError)>2)||(ABS(rightError)>2)){
            T1 = millis();
        }
    }
    chassisStop ();
}

void driveSonarLeft(
        int leftTarget, int power, float integralRange, float timeOut){
    PIDData leftData;

    float kp = 0.7;
    float ki = 0.005;
    float kd = 0.0;
    float maxPower = power;

    pidDataInit(&leftData, kp, ki, kd, maxPower, 32767, integralRange);

    int leftDist;

    long T1, T2;
    T1 = millis();
    T2 = millis();

    timeOut = timeOut*1000;

    while ((T1 > (millis() - 200))&&(T2 > (millis() - timeOut))) {

        leftDist = ultrasonicGet(leftSonar);

        if (leftDist != ULTRA_BAD_RESPONSE) {
            leftError = leftDist - leftTarget;
            leftPower = capMotorPower(
                    pidNextIteration(&leftData, leftError), maxPower);
            leftMotorsSet(leftPower);
        }

        rightMotorsSet(leftPower);

        if(ABS(leftError)>2){
            T1 = millis();
        }
    }
    chassisStop ();
}

void driveSonarRight(
        int rightTarget, int power, float integralRange, float timeOut){
    PIDData rightData;

    float kp = 0.7;
    float ki = 0.005;
    float kd = 0.0;
    float maxPower = power;

    pidDataInit(&rightData, kp, ki, kd, maxPower, 32767, integralRange);

    int rightDist;

    long T1, T2;
    T1 = millis();
    T2 = millis();

    timeOut = timeOut*1000;

    while ((T1 > (millis() - 200))&&(T2 > (millis() - timeOut))) {

        rightDist = ultrasonicGet(rightSonar);

        if (rightDist != ULTRA_BAD_RESPONSE) {
            rightError = rightDist - rightTarget;
            rightPower = capMotorPower(
                    pidNextIteration(&rightData, rightError), maxPower);
            rightMotorsSet(rightPower);
        }

        leftMotorsSet(rightPower);

        if(ABS(rightError)>2){
            T1 = millis();
        }
    }
    chassisStop ();
}

