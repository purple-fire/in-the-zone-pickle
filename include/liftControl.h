/**
 * Functions for controlling the lift
 */

#include "main.h"

#define MOGO_DOWN 1290
#define MOGO_HALF 1860
#define MOGO_UP 2930

#define CONE_DOWN 3920
#define CONE_HALF 3000
#define CONE_UP 1765

#define LIFT_DOWN 3920
#define LIFT_HALF 3000
#define LIFT_UP 1765

void liftControl(void *parameter);
int getLiftState();

void setMogoAngle(int liftAngle);
void setConeAngle(int liftAngle);
void setLiftState(int state);
void setLiftHeight(int liftAngle);
