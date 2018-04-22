/**
 * @file auto_test.c
 * @author Zach Peltzer
 * @date Created: Sat, 21 Apr 2018
 * @date Last Modified: Sat, 21 Apr 2018
 */

#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void autoTest() {
    devgyroResetTo(&gyroDev, 0);

    //Begin!

    resetStackCones();
    grabState = GRABBED_STACK; /* Pre-load cone */
    liftToggle = 0;
    coneToggle = 1;
    mogoToggle = 1;

    baseTurn(45, true, true, 4.0);
    delay(3000);
    baseTurn(135, true, true, 4.0);
    delay(3000);
    baseTurn(-135, true, true, 4.0);

    delay(3000);
    return;
}

/* vim: set tw=80 ft=c: */
