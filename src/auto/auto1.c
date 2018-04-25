#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void auto1(){
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

    //Pick up the fourth cone
    baseTurn(0, 1, 1, 1.5);
    setConeAngleBlock(CONE_HALF,500);
    baseControl(11, 80, 10, 1.0);
    pickupCone(0);
    stackCone();
    dropCone();

    baseControl(-54, 80, 10, 2.0);

    baseTurn(45, 1.0, 0.2, 1.0);

    //Line Up With 20Pt

    baseControl(-19, 80, 10, 1.5);
    baseTurn(135, 1, 1, 1.5);

    //Previous Value: 1.0, 0.3. Not far enough forward.

    //Score in 20Pt

    setMogoAngle(MOGO_DROP_HIGH);
    driveTime(127, 127, true, 1.4);
    driveTime(-127, -127, false, 0.6);

    resetStackCones();
    devgyroResetTo(&gyroDev, 135);

    //Line up with the second Mogo
    delay (400);

    baseTurn(360-112, 1, 1, 2.1);
    setMogoAngleBlock(MOGO_DOWN, 500);
    baseControl(39, 80, 10, 2.0);
    baseTurn(360-109,1,1,1.5);
    baseControl(21,80,10,1.0);
    setMogoAngleBlock(MOGO_UP, 1000);
    baseTurn(360-100,1.0,1.0,1000);
    /*
    baseControl(-12, 80, 10, 2.0);
    baseTurn(180, 1, 1, 2.0);

    loaderAlign(40, 2000);

    baseTurn(180,1,1,.5);

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

*/
    baseControl(-47, 80, 10, 2.0);
    baseTurn(135, 1, 1, 1.5);
    barBump(60,3.0);

    setMogoAngleBlock(MOGO_DROP_LOW,750);

    baseControl(-24, 80, 10, 1.5);

    return;
}
