#include <limits.h>

#include "main.h"
#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

#ifdef __cplusplus
extern "C" {
#endif

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
void autonOne();

#ifdef __cplusplus
}
#endif
