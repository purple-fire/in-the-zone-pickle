/**
 * Functions for controlling the lift
 */

#include "main.h"


#define MOGO_DOWN 1100
#define MOGO_HALF 2000
#define MOGO_DROP_HIGH 2000
#define MOGO_DROP_LOW 1290
#define MOGO_UP 2760
#define MOGO_THRESH 10

#define LIFT_DOWN 20
#define LIFT_LOADER 750
#define LIFT_INC_HALF 150
#define LIFT_INC_FULL 300
#define LIFT_THRESH 50

#define CONE_START 1060
#define CONE_DOWN 3110
#define CONE_HALF 2580
#define CONE_UP 1160
#define CONE_UP_OFFSET 1800
#define CONE_THRESH 25

#define CONE_SCORE 2026

#define GOLIATH_IN 64
#define GOLIATH_OUT -127

#define CONE_COUNT_MAX 16

typedef enum {
    GRABBED_NONE,
    GRABBED_CONE,
    GRABBED_STACK,
} GrabState;

typedef struct {
    /**
     * Lift position before moving dropping
     */
    int liftPosPre;

    /**
     * Lift position to drop the cone to.
     */
    int liftPosPost;

    /**
     * Cone lift position (not quite CONE_UP as the stack grows)
     */
     int conePos;
} ConeStackPos;

/**
 * Lift positions for each cone in the stack. Indexed by the current number of
 * cones in the stack (not including the cone being stacked).
 */
extern ConeStackPos coneStackPositions[CONE_COUNT_MAX];

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

extern GrabState grabState;

void liftControl(void *parameter);

void moveConeGround();
void moveConeLoader();

void incrementNumCones();
void decrementNumCones();
void resetCones();

void setMogoAngle(int liftAngle);
void setConeAngle(int liftAngle);
void setLiftHeight(int liftAngle);

bool stackCone();
bool pickupCone(int mode);
bool ungrabStack();

/* Blocking versions of lift setters */
bool setLiftHeightBlock(int angle, int timeout);
bool setMogoAngleBlock(int angle, int timeout);
bool setConeAngleBlock(int angle, int timeout);

