/**
 * Functions for controlling the lift
 */

#include "main.h"

#define MOGO_DOWN 1290
#define MOGO_HALF 1860
#define MOGO_UP 2750

#define CONE_DOWN 3920
#define CONE_HALF 3000
#define CONE_UP 1765

#define LIFT_DOWN 3920
#define LIFT_HALF 3000
#define LIFT_UP 1765

void liftControl(void *parameter);

void setMogoAngle(int liftAngle);
void setConeAngle(int liftAngle);
int getConeAngle();
void setLiftHeight(int liftAngle);
