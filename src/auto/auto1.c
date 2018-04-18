#include "main.h"
#include "auto_utils.h"

#include "utilities.h"
#include "pid.h"
#include "motor.h"
#include "liftControl.h"

void autonOne(){
  devgyroResetTo(&gyroDev, -4);

 //NOW START!

 mogoToggle = 1;
 setMogoAngle(MOGO_DOWN);
 delay(700);

 //FIRST BASE
 baseControl(55, 80, 10, 2.0);
 setMogoAngle(MOGO_UP);
 delay(500);
 baseControl(-48, 80, 10, 2.0);
 baseTurn(-135, true, true, 1.5);

 //Line Up With 20Pt
 baseControl(16, 80, 10, 2.0);
 baseTurn(-225, true, true, 3);
 devgyroOffset(&gyroDev, -360);

 //Score in 20Pt
 driveTime(127, 127, true, 1.0);
 setMogoAngle(MOGO_DROP_HIGH);
 driveTime(-127, -127, false, 0.3);

//BELOW IS SKILLS BOT AUTO!!!
/*
 //SECOND BASE
 baseControl(-6, 80, 10, 1.5);
 baseTurn(-45, true, true, 1.5);
 baseControl(-14, 80, 10, 1.5);
 baseTurn(45, true, true, 1.5);
 setMogoAngle(MOGO_DOWN);
 delay(400);
 baseControl(19, 80, 10, 2.5);
 setMogoAngle(MOGO_UP);
 delay(500);
 baseTurn(360-135, true, true, 2.0);
 devgyroOffset(&gyroDev, -360);
 baseControl(17, 80, 10, 2.0);
 wallBump(10, 25, 2.0, -135);

 setMogoAngle(MOGO_HALF);
 delay(300);
 baseControl(-8, 80, 10, 2.0);
 setMogoAngle(MOGO_UP);

 //THIRD BASE
 baseTurn(-45, true, true, 2.0);
 baseControl(30, 80, 10, 2.5);
 baseTurn(45, true, true, 2.0);
 setMogoAngle(MOGO_DOWN);
 delay(500);
 baseControl(32, 80, 10, 2.5);
 setMogoAngle(MOGO_UP);
 delay(500);
 baseTurn(-135, true, true, 2.0);
 baseControl(32, 80, 10, 2.5);

 setMogoAngle(MOGO_HALF);
 delay(500);
 baseControl(-25, 80, 10, 2.0);

 //FOURTH BASE
 baseTurn(45, true, true, 1.5);
 setMogoAngle(MOGO_DOWN);
 baseControl(62, 10, 10, 2.5);
 setMogoAngle(MOGO_UP);
 delay(300);

 baseTurn(90, true, true, 1.5);
 baseControl(20, 10, 10, 2.0);
 baseTurn(45, true, true, 0.7);

 driveTime(127, 127, true, 1.0);
 driveTime(-127, -127, false, 0.5);

 // FIFTH BASE
 baseTurn(-40, true, true, 1.5);
 baseControl(-24, 80, 10, 2.5);
 baseTurn(-135, true, true, 1.5);
 setMogoAngle(MOGO_DOWN);
 delay(300);
 baseControl(19, 80, 10, 2.5);
 setMogoAngle(MOGO_UP);
 baseTurn(45-360, true, true, 2.0);
 devgyroOffset(&gyroDev, 360);
 baseControl(24, 80, 10, 2.0);
 wallBump(10, 30, 3, 45);

 setMogoAngle(MOGO_HALF);
 delay(200);
 baseControl(-8, 80, 10, 2.5);
 setMogoAngle(MOGO_UP);

 // SIXTH BASE
 baseTurn(135, true, true, 2);
 baseControl(-46, 80, 10, 2.0);

 baseTurn(360-90, true, true, 2.0);
 devgyroOffset(&gyroDev, -360);
 setMogoAngle(MOGO_DOWN);
 baseControl(50, 80, 10, 2.0);
 setMogoAngle(MOGO_UP);
 delay(400);
 baseControl(-50, 80, 10, 2.0);
 baseTurn(45, true, false, 0.5);

 wallBump(10, 25, 3, 45);

 //Bump Bar
 setMogoAngle(MOGO_HALF);
 delay(500);
 baseControl(-10, 80, 10, 2.0);
 setMogoAngle(MOGO_UP);
 baseTurn(95, true, true, 2.0);
 baseControl(-50, 80, 10, 2.0);

*/
 delay(2000);
 return;
}
