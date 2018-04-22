/**
 * Functions for controlling the lift
 */

#include "main.h"


#define MOGO_DOWN 1100
#define MOGO_HALF 2000
#define MOGO_DROP_HIGH 2000
#define MOGO_DROP_LOW 1290
#define MOGO_UP 2690
#define MOGO_THRESH 10

#define LIFT_DOWN 40
#define LIFT_MID 250
#define LIFT_LOADER 550
#define LIFT_STATIONARY 900
#define LIFT_THRESH 50

#define CONE_START 1060
#define CONE_DOWN 3110
#define CONE_HALF 2500
#define CONE_LOADER 1850
#define CONE_UP 1160
#define CONE_UP_OFFSET 1800
#define CONE_THRESH 25

#define CONE_SCORE 2026

#define GOLIATH_IN 64
#define GOLIATH_OUT -127

#define STACK_CONES_MAX 16
#define STATIONARY_CONES_MAX 8

typedef enum {
    GRABBED_NONE,
    GRABBED_CONE,
    GRABBED_STACK,
    GRABBED_STATIONARY,
} GrabState;

/**
 * Lift positions for each cone in the stack. Indexed by the current number of
 * cones in the stack (not including the cone being stacked).
 */
 extern int stackingMode;

typedef struct StackConePos {
    int liftPosPre;
    int liftPosPost;
    int conePos;
} StackConePos;
extern StackConePos stackConePositions[STACK_CONES_MAX];

typedef struct StationaryConePos {
    int liftPos;
    int conePos;
} StationaryConePos;
extern StationaryConePos stationaryConePositions[STATIONARY_CONES_MAX];

extern int stackConeCount;
extern int stationaryConeCount;
extern int mogoTarget;
extern int mogoPosition;
extern int mogoToggle;

extern int coneTarget;
extern int conePosition;
extern int coneToggle;

extern int liftTarget;
extern int liftPosition;
extern int liftToggle;

extern GrabState grabState;

void liftControl(void *parameter);

void moveConeGround();
void moveConeLoader();

void incStackCones();
void decStackCones();
void resetStackCones();

void incStationaryCones();
void decStationaryCones();
void resetStationaryCones();

void setMogoAngle(int liftAngle);
void setConeAngle(int liftAngle);
void setLiftHeight(int liftAngle);

void changeStackingMode (int mode);
bool stackCone();
bool stackConeStationary();
bool stackConeLoader();
bool pickupCone(int mode);
bool pickupConeLoader(int mode);
void dropCone();
bool grabStack(int mode);
bool ungrabStack();

/**
 * Raise the lift to allow moving the mogo lift
 */
void liftClearMogo();

/* Blocking versions of lift setters */
bool setLiftHeightBlock(int angle, int timeout);
bool setMogoAngleBlock(int angle, int timeout);
bool setConeAngleBlock(int angle, int timeout);

