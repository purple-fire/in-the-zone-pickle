/**
 * @file auto_test.c
 * @author Zach Peltzer
 * @date Created: Sat, 21 Apr 2018
 * @date Last Modified: Mon, 23 Apr 2018
 */

#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void autoTest() {

    devgyroResetTo(&gyroDev, 0);
    /**

    //Begin!

    resetStackCones();
    grabState = GRABBED_STACK;
    liftToggle = 0;
    coneToggle = 1;
    mogoToggle = 1;

    baseTurn(45, true, true, 4.0);
    delay(3000);
    baseTurn(135, true, true, 4.0);
    delay(3000);
    baseTurn(-135, true, true, 4.0);

    delay(3000);
    */

    baseTurn(-90, false, true, 3.0);
    delay(4000);
    baseTurn(-40, true, true, 2.0);

    /*
    barBump(30, 3.0);
    devgyroResetTo(&gyroDev, 135);

    baseTurn(360-135, true, true, 2.5);
    delay(500);
    driveSonarRight(12, 80, 10, 2.0);
    delay(500);
    baseTurn(360-90, true, true, 2.5);
    setMogoAngleBlock(MOGO_DOWN, 500);
    baseControl(42, 80, 10, 2.0);
    */

    return;
}

/* vim: set tw=80 ft=c: */
