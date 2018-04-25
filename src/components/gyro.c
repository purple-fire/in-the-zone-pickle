/**
 * @file gyro.c
 * @author Zach Peltzer
 * @date Created: Thu, 01 Mar 2018
 * @date Last Modified: Tue, 24 Apr 2018
 */

#include "gyro.h"
#include "utilities.h"

void devgyroInit(DevGyro *pGyro, unsigned char port, unsigned short multiplier) {
    pGyro->base = gyroInit(port, multiplier);
    pGyro->offset = 0;
    delay(2000); /* Gyro needs to time to callibrate */
}

void devgyroDestroy(DevGyro *pGyro) {
    gyroShutdown(pGyro->base);
}

int devgyroGet(DevGyro *pGyro) {
    return gyroGet(pGyro->base) + pGyro->offset;
}

void devgyroReset(DevGyro *pGyro) {
    devgyroResetTo(pGyro, 0);
}

void devgyroResetTo(DevGyro *pGyro, int angle) {
    gyroReset(pGyro->base);

    /* Auto routines are made for the blue side of the field and the angles are
     * just flipped for the red side */
    pGyro->offset = teamColor == BLUE_TEAM ? angle : -angle;
}

void devgyroOffset(DevGyro *pGyro, int offset) {
    pGyro->offset += offset;
}

/* vim: set tw=80 ft=c: */
