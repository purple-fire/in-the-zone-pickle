#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void autonOne(){
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
    baseTurn(0, true, true, 1.5);
    baseControl(7, 80, 10, 1.0);
    pickupCone(0);
    stackCone();
    //Previous Value: 48. Relative to 55 inches above.
    //Back up.

    //Score yet another cone
    baseTurn(0, true, true, 1.5);
    baseControl(11, 80, 10, 1.0);
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

    baseControl(-42, 80, 10, 2.0);
    baseTurn(45, true, false, 1.0);

    //Line Up With 20Pt

    baseControl(-16, 80, 10, 1.5);
    baseTurn(135, true, true, 1.5);

    //Previous Value: 1.0, 0.3. Not far enough forward.

    //Score in 20Pt

    setMogoAngle(MOGO_DROP_HIGH);
    driveTime(127, 127, true, 1.4);
    driveTime(-127, -127, false, 0.6);

    resetStackCones();
    devgyroResetTo(&gyroDev, 135);

    //Line up with the second Mogo
    delay (400);

    baseTurn(360-112, true, true, 2.1);
    setMogoAngleBlock(MOGO_DOWN, 500);
    baseControl(42, 80, 10, 2.0);
    baseTurn(360-110,true,true,1.5);
    baseControl(17,80,10,1.0);
    setMogoAngleBlock(MOGO_UP, 1000);
    baseControl(-12, 80, 10, 2.0);
    baseTurn(180, true, true, 2.0);

    loaderAlign(40, 2000);

    baseTurn(180,true,true,.5);

    bool finishedACone = false;

    while (millis() - startTime < 38000) {

        pickupConeLoader(0);
        stackConeLoader();
        if (millis() - startTime > 38000){
          break;
        }
        ungrabStack();

        delay(500);
    }


    baseTurn(90, true, true, 1.0);
    setMogoAngle(MOGO_DOWN);
    baseControl(7,80,10,1.0);
    baseControl(-24, 80, 10, 1.5);

    return;
}
