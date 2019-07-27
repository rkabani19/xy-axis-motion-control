/* DESCRIPTION
 * Sample code structure for PWM Control of an LED
*/


#include "driverlib.h"
#include "stdbool.h"


void setupMotors();
void controlMotor(int code, int is_motor1, int isReversed);
void controlMotor1Bool(bool a, bool b, bool c, bool d);
void controlMotor2Bool(bool a, bool b, bool c, bool d);
void runStepper(int is_motor1, int isReversed, int isNormalSpeed);
