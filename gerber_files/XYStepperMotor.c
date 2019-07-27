/* DESCRIPTION
 * Sample code structure for PWM Control of an LED
 */

#include "XYStepperMotor.h"


#define MOTOR1A_PORT GPIO_PORT_P5
#define MOTOR1A_PIN GPIO_PIN0

#define MOTOR1B_PORT GPIO_PORT_P1
#define MOTOR1B_PIN GPIO_PIN6

#define MOTOR1C_PORT GPIO_PORT_P1
#define MOTOR1C_PIN GPIO_PIN7

#define MOTOR1D_PORT GPIO_PORT_P5
#define MOTOR1D_PIN GPIO_PIN2

#define MOTOR2A_PORT GPIO_PORT_P5
#define MOTOR2A_PIN GPIO_PIN3

#define MOTOR2B_PORT GPIO_PORT_P1
#define MOTOR2B_PIN GPIO_PIN0

#define MOTOR2C_PORT GPIO_PORT_P2
#define MOTOR2C_PIN GPIO_PIN7

#define MOTOR2D_PORT GPIO_PORT_P2
#define MOTOR2D_PIN GPIO_PIN5



#define completePeriod 511
bool increasing;
int highPeriod;

const int A = 1000;
const int AB = 1100;
const int B = 0100;
const int BC = 0110;
const int C_ = 0010;
const int CD = 0011;
const int D = 0001;
const int DA = 1001;

void setupMotors() {
    // Motor 1
    GPIO_setAsOutputPin(
            MOTOR1A_PORT,
            MOTOR1A_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR1B_PORT,
            MOTOR1B_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR1C_PORT,
            MOTOR1C_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR1D_PORT,
            MOTOR1D_PIN
    );

    // Motor 2
    GPIO_setAsOutputPin(
            MOTOR2A_PORT,
            MOTOR2A_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR2B_PORT,
            MOTOR2B_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR2C_PORT,
            MOTOR2C_PIN
    );

    GPIO_setAsOutputPin(
            MOTOR2D_PORT,
            MOTOR2D_PIN
    );

}

void controlMotor(int code, int is_motor1, int isReversed){
    bool codearray[4]= {false,false,false,false};
    int i=0;
    while(i<4){
        codearray[i] = code%10;
        code = code/10;
        i++;
    }

    if (!isReversed) {
        if (is_motor1) {
            controlMotor1Bool(codearray[3],codearray[2],codearray[1],codearray[0]);
        } else {
            controlMotor2Bool(codearray[3],codearray[2],codearray[1],codearray[0]);
        }
    } else {
        if (is_motor1) {
            controlMotor1Bool(codearray[0],codearray[1],codearray[2],codearray[3]);
        } else {
            controlMotor2Bool(codearray[0],codearray[1],codearray[2],codearray[3]);
        }
    }
}

void controlMotor1Bool(bool a, bool b, bool c, bool d) {
    if (a) {
        GPIO_setOutputHighOnPin(
                MOTOR1A_PORT,
                MOTOR1A_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR1A_PORT,
                MOTOR1A_PIN
        );
    }

    if (b) {
        GPIO_setOutputHighOnPin(
                MOTOR1B_PORT,
                MOTOR1B_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR1B_PORT,
                MOTOR1B_PIN
        );
    }

    if (c) {
        GPIO_setOutputHighOnPin(
                MOTOR1C_PORT,
                MOTOR1C_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR1C_PORT,
                MOTOR1C_PIN
        );
    }

    if (d) {
        GPIO_setOutputHighOnPin(
                MOTOR1D_PORT,
                MOTOR1D_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR1D_PORT,
                MOTOR1D_PIN
        );
    }
}

void controlMotor2Bool(bool a, bool b, bool c, bool d) {
    if (a) {
        GPIO_setOutputHighOnPin(
                MOTOR2A_PORT,
                MOTOR2A_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR2A_PORT,
                MOTOR2A_PIN
        );
    }

    if (b) {
        GPIO_setOutputHighOnPin(
                MOTOR2B_PORT,
                MOTOR2B_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR2B_PORT,
                MOTOR2B_PIN
        );
    }

    if (c) {
        GPIO_setOutputHighOnPin(
                MOTOR2C_PORT,
                MOTOR2C_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR2C_PORT,
                MOTOR2C_PIN
        );
    }

    if (d) {
        GPIO_setOutputHighOnPin(
                MOTOR2D_PORT,
                MOTOR2D_PIN
        );
    } else {
        GPIO_setOutputLowOnPin(
                MOTOR2D_PORT,
                MOTOR2D_PIN
        );
    }
}

void runStepper(int is_motor1, int isReversed, int isNormalSpeed)
{
    if (isNormalSpeed) {
        const int delay = 2000;
        controlMotor(AB, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(BC, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(CD, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(DA, is_motor1, isReversed);
        _delay_cycles(delay);
    } else {
        const int delay = 300;
        controlMotor(AB, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(BC, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(CD, is_motor1, isReversed);
        _delay_cycles(delay);
        controlMotor(DA, is_motor1, isReversed);
        _delay_cycles(delay);
    }

}

//void runStepper(int is_motor1)
//{
//    const int delay = 500;
//    controlMotor(A, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(AB, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(B, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(BC, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(C, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(CD, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(D, is_motor1);
//    _delay_cycles(delay);
//    controlMotor(DA, is_motor1);
//    _delay_cycles(delay);
//}

