#include <limits.h>

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOADER_DISTANCE 37

int tickGoal;

int rightError;
int rightPower;

int leftError;
int leftPower;

int turnError;
int turnPower;

int gyroValue;

/* TODO Figure out a way to update these in a thread-safe fashion when
 * baseControl() is not running
 */
int rightEncoderValue;
int leftEncoderValue;

void baseControl(float target, float power, float integralRange, float timeOut);
void baseTurn(float target, float leftScale, float rightScale, float timeOut);
void driveTime(float powerL, float powerR, bool coast, float timeOut);
void wallBump(int threshold, float power, float timeOut, int angle);
void loaderAlign(int power, float timeout);
void driveSonar(
        int leftTarget, int rightTarget, int power, float integralRange, float timeOut);
void driveSonarLeft(
        int leftTarget, int power, float integralRange, float timeOut);
void driveSonarRight(
        int rightTarget, int power, float integralRange, float timeOut);

/**
 * Move forward (usually at a low speed) until the encoders become steady (the
 * robot has hit something).
 */
void barBump(int power, float timeout);

void auto1();
void auto2();
void autoTest();

void auto1Blue();
void auto2Blue();
void auto1Red();
void auto2Red();

#ifdef __cplusplus
}
#endif
