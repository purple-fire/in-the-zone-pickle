#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void auto2() {
    int startTime = millis();

    devgyroResetTo(&gyroDev, 0);

    //Begin!

    resetStackCones();
    grabState = GRABBED_STACK; /* Pre-load cone */
    liftToggle = 0;
    coneToggle = 1;
    mogoToggle = 1;

    setMogoAngleBlock(MOGO_DOWN, 750);
    //FIRST BASE

    //Past Distance: 55. Note: Too far. Pushes cone
    baseControl(45, 80, 10, 2.0);
    setMogoAngleBlock(MOGO_UP, 1500);

    //Score preload cone
    incStackCones();
    dropCone();

    //Score another cone
    baseControl(7, 80, 10, 1.0);
    pickupCone(0);
    stackCone();
    //Previous Value: 48. Relative to 55 inches above.
    //Back up.

    //Score yet another cone
    baseControl(10, 80, 10, 1.0);
    dropCone();
    pickupCone(0);
    stackCone();
    dropCone();

    /*
    //Score yet another yet another cone
    baseControl(9, 80, 10, 1.0);
    dropCone();
    pickupCone(0);
    stackCone();
    dropCone();
    */

    baseControl(-24, 80, 10, 2.0);
    baseTurn(-90, 0.25, 1.0, 3.0);

    /* Robot spazzes out with this for some reason
    driveSonar(LOADER_DISTANCE, LOADER_DISTANCE, 80, 10, 2.0);
    */

    /* baseControl(-6, 800, 10, 2.0); */

    bool finishedACone = false;

    do {
        pickupConeLoader(0);
        stackConeLoader();
        if (millis() - startTime > 38000){
            /* Break out of the loop here so that we end with the stack grabbed
             */
            break;
        }
        ungrabStack();

        delay(500);
    } while (1);

    baseControl(-20, 80, 10, 2.0);
    baseTurn(-135, true, true, 2.0);
    baseControl(36, 80, 10, 2.0);

    setMogoAngle(MOGO_DROP_LOW);
    if (stackConeCount > 5){
        liftToggle = 0;
        setConeAngleBlock(CONE_UP_OFFSET, 700);

        motorSet(goliathMotor, GOLIATH_OUT);
        motorSet(liftMotor, -127);
        motorSet(liftMotorAux, -127);
        delay(600);
        motorSet(liftMotor, 0);
        motorSet(liftMotorAux, 0);
        motorSet(goliathMotor, 0);
    }

    resetStackCones();
    char rainbow = 'R';
    fputc(rainbow,uart1);

    baseControl(-8, 80, 10, 2.0);
}

