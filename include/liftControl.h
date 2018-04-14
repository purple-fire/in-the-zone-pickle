/**
 * Functions for controlling the lift
 */

#include "main.h"


#define MOGO_DOWN 1100
#define MOGO_HALF 2000
#define MOGO_DROP_HIGH 2000
#define MOGO_DROP_LOW 1290
#define MOGO_UP 2760

#define CONE_DOWN 3920
#define CONE_HALF 3000
#define CONE_UP 1690
#define CONE_UP_OFFSET 1800
#define LIFT_DOWN 550
#define LIFT_LOADER 750
#define LIFT_INC_HALF 150
#define LIFT_INC_FULL 300

#define GOLIATH_IN 64
#define GOLIATH_OUT -127

extern int numCones;
extern int backupCones;
extern int mogoTarget;
extern int mogoPosition;
extern int mogoToggle;

extern int coneTarget;
extern int conePosition;
extern int coneToggle;

extern int liftTarget;
extern int liftPosition;
extern int liftToggle;

void liftControl(void *parameter);
void moveConeGround();
void moveConeLoader();
void stackCone();
void resetCones();
void decrementCone();
void pickupCone(int mode);
void setMogoAngle(int liftAngle);
void setConeAngle(int liftAngle);
int getConeAngle();
void setLiftHeight(int liftAngle);

