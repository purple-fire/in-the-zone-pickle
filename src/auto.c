/** @file auto.c
 * @brief File for autonomous code
 *
 * This file should contain the user autonomous() function and any functions
 * related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */
#include "main.h"
#include "auto_utils.h"
#include "motor.h"

void autonomous ()
{
  int autoChoice = 2;

  switch(autoChoice) {
   case 1:
      autonOne();
      break;
   case 2:
     autoTest();
      break;
   default :
      return;
  }

  return;
}
