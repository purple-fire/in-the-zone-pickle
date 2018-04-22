/**
 * @file utilities.c
 * @author Zach Peltzer
 * @date Created: Thu, 19 Apr 2018
 * @date Last Modified: Sat, 21 Apr 2018
 */

#include "utilities.h"
#include "liftControl.h"

int teamColor = BLUE_TEAM;

void ledSendConeCount(int count) {
    if (count > STACK_CONES_MAX) {
        return;
    }

    fputc(teamColor, LED_SERIAL_PORT);
    fputc(count + 'a', LED_SERIAL_PORT);
}

/* vim: set tw=80 ft=c: */
