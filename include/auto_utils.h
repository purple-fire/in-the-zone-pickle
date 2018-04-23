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
void baseTurn(float target, bool leftToggle, bool rightToggle, float timeOut);
void driveTime(float powerL, float powerR, bool coast, float timeOut);
void wallBump(int threshold, float power, float timeOut, int angle);
void loaderAlign(int power, int timeout);
void driveUntilSonar(int target, int power, float integralRange, int timeOut);
void autonOne();
void autoTest();

#ifdef __cplusplus
}
#endif
