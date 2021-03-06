/** @file main.h
 * @brief Header file for global functions
 *
 * Any experienced C or C++ programmer knows the importance of header files. For those who
 * do not, a header file allows multiple files to reference functions in other files without
 * necessarily having to see the code (and therefore causing a multiple definition). To make
 * a function in "opcontrol.c", "auto.c", "main.c", or any other C file visible to the core
 * implementation files, prototype it here.
 *
 * This file is included by default in the predefined stubs in each VEX Cortex PROS Project.
 *
 * Copyright (c) 2011-2016, Purdue University ACM SIGBots.
 * All rights reserved.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "API.h"
#include "gyro.h"

/* Allow usage of this file in C++ programs */
#ifdef __cplusplus
extern "C" {
#endif

/* Sensors */


//Digital Port Macros
#define QUAD_TOP_PORT_RIGHT 3
#define QUAD_BOTTOM_PORT_RIGHT  4
#define QUAD_TOP_PORT_LEFT 5
#define QUAD_BOTTOM_PORT_LEFT  6
#define ULTRASONIC_LEFT_PING 7
#define ULTRASONIC_LEFT_ECHO 8
#define ULTRASONIC_RIGHT_PING 10
#define ULTRASONIC_RIGHT_ECHO 9
//Analog Port Macros
#define MOGO_POT_PORT 2
#define LIFT_POT_PORT 3
#define CONE_POT_PORT 5
#define AUTO_SELECTOR_POT_PORT 6
#define GYRO_PORT 8

//Joystick Input Macros
#define MOGO_UP_BUTTON joystickGetDigital(1, 6, JOY_UP)
#define MOGO_DOWN_BUTTON joystickGetDigital(1, 6, JOY_DOWN)
#define MOGO_DROP_LOW_BUTTON joystickGetDigital(1, 5, JOY_UP)
#define MOGO_DROP_HIGH_BUTTON joystickGetDigital(1, 5, JOY_DOWN)

#define CONE_ARM_DOWN_BUTTON joystickGetDigital(2, 5, JOY_DOWN)
#define CONE_ARM_UP_BUTTON joystickGetDigital(2, 6, JOY_DOWN)
#define LIFT_UP_BUTTON joystickGetDigital(2, 8, JOY_UP)
#define LIFT_DOWN_BUTTON joystickGetDigital(2, 8, JOY_DOWN)
#define MANUAL_LIFT_UP_BUTTON joystickGetDigital(2, 6, JOY_UP)

#define STACKING_MODE_CHANGE_BUTTON joystickGetDigital(2, 8, JOY_LEFT)

#define GOLIATH_IN_BUTTON joystickGetDigital(2, 7, JOY_UP)
#define GOLIATH_OUT_BUTTON joystickGetDigital(2, 7, JOY_DOWN)

#define MODE_SWITCH_LEFT_BUTTON joystickGetDigital(1, 7, JOY_LEFT)
#define MODE_SWITCH_RIGHT_BUTTON joystickGetDigital(1, 7, JOY_RIGHT)

#define LIFT_CONTROL_SWITCH joystickGetDigital(1, 7, JOY_UP)
#define CONE_DEC_BUTTON joystickGetDigital(2, 7, JOY_LEFT)
#define CONE_INC_BUTTON joystickGetDigital(2, 7, JOY_RIGHT)

extern Ultrasonic leftSonar;
extern Ultrasonic rightSonar;
extern Encoder BLEncoder;
extern Encoder BREncoder;

extern DevGyro gyroDev;

/* Globals so that values can be seen from outside functions. */
extern int tickGoal,
           rightError,
           rightPower,
           leftError,
           leftPower,
           turnError,
           turnPower,

           gyroValue,
           gyroOffset,
           rightEncoderValue,
           leftEncoderValue,

           lineThreshold;

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. If the robot is disabled or communications is lost, the autonomous task
 * will be stopped by the kernel. Re-enabling the robot will restart the task,
 * not re-start it from where it left off.
 *
 * Code running in the autonomous task cannot access information from the VEX
 * Joystick. However, the autonomous function can be invoked from another task
 * if a VEX Competition Switch is not available, and it can access joystick
 * information if called in this way.
 *
 * The autonomous task may exit, unlike operatorControl() which should never
 * exit. If it does so, the robot will await a switch to another mode or
 * disable/enable cycle.
 */
void autonomous();

/**
 * Runs pre-initialization code. This function will be started in kernel mode
 * one time while the VEX Cortex is starting up. As the scheduler is still
 * paused, most API functions will fail.
 *
 * The purpose of this function is solely to set the default pin modes
 * (pinMode()) and port states (digitalWrite()) of limit switches, push buttons,
 * and solenoids. It can also safely configure a UART port (usartOpen()) but
 * cannot set up an LCD (lcdInit()).
 */
void initializeIO();

/**
 * Runs user initialization code. This function will be started in its own task
 * with the default priority and stack size once when the robot is starting up.
 * It is possible that the VEXnet communication link may not be fully
 * established at this time, so reading from the VEX Joystick may fail.
 *
 * This function should initialize most sensors (gyro, encoders, ultrasonics),
 * LCDs, global variables, and IMEs.
 *
 * This function must exit relatively promptly, or the operatorControl() and
 * autonomous() tasks will not start. An autonomous mode selection menu like the
 * pre_auton() in other environments can be implemented in this task if desired.
 */
void initialize();

/**
 * Runs the user operator control code. This function will be started in its own
 * task with the default priority and stack size whenever the robot is enabled
 * via the Field Management System or the VEX Competition Switch in the operator
 * control mode. If the robot is disabled or communications is lost, the
 * operator control task will be stopped by the kernel. Re-enabling the robot
 * will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the
 * VEX Cortex will run the operator control task. Be warned that this will also
 * occur if the VEX Cortex is tethered directly to a computer via the USB A to A
 * cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is
 * available and the scheduler is operational. However, proper use of delay() or
 * taskDelayUntil() is highly recommended to give other tasks (including system
 * tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop,
 * even if empty.
 */
void operatorControl();

#ifdef __cplusplus
}
#endif

#endif
