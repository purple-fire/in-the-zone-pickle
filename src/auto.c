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

#define AUTO_COUNT 6

struct {
    int max, min;
    void (*autoFunc)(void);
} autoPositions[AUTO_COUNT] = {
    { 4095, 3750, auto1 },
    { 3750, 2850, auto2 },
    { 2850, 2050, NULL },
    { 2050, 1200, NULL },
    { 1200, 500,  NULL },
    { 500,  0,    autoTest },
};

void autonomous ()
{
  int autoChoice = analogRead(AUTO_SELECTOR_POT_PORT);

  for (int i = 0; i < AUTO_COUNT; i++) {
      if (autoPositions[i].max >= autoChoice
              && autoChoice > autoPositions[i].min) {
          if (autoPositions[i].autoFunc) {
              autoPositions[i].autoFunc();
          } else {
              /* Just sit there if the position is undefined */
              return;
          }
      }
  }

  return;
}
