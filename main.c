#include "driverlib.h"
#include "Board.h"
#include <stdio.h>
#include "hal_LCD.h"
#include "XYStepperMotor.h"

#define GPIO_PORT_S2 GPIO_PORT_P2
#define GPIO_PIN_S2 GPIO_PIN6

#define PI2_PIN GPIO_PIN3
#define PI1_PIN GPIO_PIN4
#define PI3_PIN GPIO_PIN2
#define PI4_PIN GPIO_PIN1

void setS1();
void setS2();
void setLED1();
void setLED2();
void setLCD();
void display_dir();
void run_status();
void display();
void setPI();
void Init_GPIO(void);
void turnMotorOnOrOff();
void motorHandler(int is_motor1, int isReversed, int isNormalSpeed);
void run_specific_motor();
int readAllInputPins();
void pollingLoop();
void shutDown();
void turnON();
int readAllPIs();
void setup();

// Chars for Slow: ABCD = UDLR but for Slow speed

char dir[4] = {'U', 'D', 'L', 'R'}; // Normal = 10 RPM, Slow = 6 RPM
volatile int ON = 0;
volatile int dir_count = 0;
int photoWasInterrupted = 0;

int readBtn1() {
    return (GPIO_getInputPinValue(GPIO_PORT_S1, GPIO_PIN_S1));
}

int readBtn2() {
    return (GPIO_getInputPinValue(GPIO_PORT_S2, GPIO_PIN_S2));
}

int readPI2() {
    return (GPIO_getInputPinValue(GPIO_PORT_P1, PI2_PIN));
}

int readPI1() {
    return (GPIO_getInputPinValue(GPIO_PORT_P1, PI1_PIN));
}

int readPI3() {
    return (GPIO_getInputPinValue(GPIO_PORT_P8, PI3_PIN));
}

int readPI4() {
    return (GPIO_getInputPinValue(GPIO_PORT_P8, PI4_PIN));
}

void main(void) {
    WDT_A_hold(WDT_A_BASE);
    PMM_unlockLPM5();

    setup();

    while (1) {
        while (readBtn1() || readBtn2()) {
            _delay_cycles(1000);
        }
        turnON();
        pollingLoop();
        shutDown();
    }
}

void setup() {
    // Setup
    setLED1();
    setLED2();
    setPI();
    clearLCD();
    setS1();
    setS2();
    setupMotors();
    setLCD();
}

void pollingLoop() {
    while(1) {
        while(readAllInputPins()) {
            _delay_cycles(1000);
        }

        if (readAllPIs()) { break; }
        if (readPI1() == 0 || readPI2() == 0) {
            photoWasInterrupted = 1;
            ON = 0;
            display();
            while ((readPI1() == 0) || (readPI2() == 0)) {
                if (readAllPIs()) { break; }
            }
        }
        else if (readPI3() == 0 || readPI4() == 0) {
            photoWasInterrupted = 1;
            ON = 0;
            display();
            while ((readPI3() == 0) || (readPI4() == 0)) {
                if (readAllPIs()) { break; }
            }
        }
        else if (readBtn1() == 0) {
            if (photoWasInterrupted == 0) {
                run_status();
                run_specific_motor();
            } else {
                while (readBtn1() == 0) {}
                photoWasInterrupted = 0;
            }
        }
        else if (readBtn2() == 0) {
            while (readBtn2() == 0) {} //Wait for rising edge
            display_dir();
        }
    }
}

void shutDown() {
    clearLCD();
    displayScrollText("THANK YOU");
    clearLCD();
}

void turnON() {
    clearLCD();
    displayScrollText("WELCOME");
    clearLCD();
    display();
}

int readAllPIs() {
    return (readPI1() == 0 && readPI2() == 0 && readPI3()  == 0 && readPI4() == 0);
}

int readAllInputPins() {
    return (readPI1() && readPI2() && readPI3() && readPI4() && readBtn1() && readBtn2());
}

void run_specific_motor() {
    if (dir[dir_count/2] == 'U') {
        motorHandler(1, 0, dir_count%2);
    } else if (dir[dir_count/2] == 'D') {
        motorHandler(1, 1, dir_count%2);
    } else if (dir[dir_count/2] == 'R') {
        motorHandler(0, 0, dir_count%2);
    } else if (dir[dir_count/2] == 'L') {
        motorHandler(0, 1, dir_count%2);
    }
}


void setPI() {
    //Set PhotoInterruptor #2
    GPIO_setAsInputPin(GPIO_PORT_P1, PI2_PIN);
    GPIO_setAsInputPin(GPIO_PORT_P1, PI1_PIN);
    GPIO_setAsInputPin(GPIO_PORT_P8, PI3_PIN);
    GPIO_setAsInputPin(GPIO_PORT_P8, PI4_PIN);
    //    GPIO_enableInterrupt(GPIO_PORT_P1, PI2_PIN);                                    //S1 interrupt enabled
    //    GPIO_selectInterruptEdge(GPIO_PORT_P1, PI2_PIN, GPIO_HIGH_TO_LOW_TRANSITION);   //S1 Hi/Lo edge
    //    GPIO_clearInterrupt(GPIO_PORT_P1, PI2_PIN);                                     //S1 IFG cleared
}

void setS1() {
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_S1, GPIO_PIN_S1);                    //Enable S1 internal resistance as pull-Up resistance
    //    GPIO_enableInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);                                    //S1 interrupt enabled
    //    GPIO_selectInterruptEdge(GPIO_PORT_S1, GPIO_PIN_S1, GPIO_HIGH_TO_LOW_TRANSITION);   //S1 Hi/Lo edge
    //    GPIO_clearInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);                                     //S1 IFG cleared

}

void setS2() {
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_S2, GPIO_PIN_S2);                    //Enable S2 internal resistance as pull-Up resistance
    //    GPIO_enableInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);                                    //S2 interrupt enabled
    //    GPIO_selectInterruptEdge(GPIO_PORT_S2, GPIO_PIN_S2, GPIO_LOW_TO_HIGH_TRANSITION);   //S2 Hi/Lo edge
    //    GPIO_clearInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);                                     //S2 IFG cleared
}

void setLED1() {
    GPIO_setAsOutputPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
}

void setLED2() {
    GPIO_setAsOutputPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
}

void setLCD() {
    Init_LCD();
}

void display() {
    clearLCD();
    showChar((char) dir[dir_count/2], pos6);
    if (dir_count % 2 == 0) { //Slow
        showChar((char) '6', pos5);
    } else { //Normal
        showChar((char) '0', pos5);\
        showChar((char) '1', pos4);
    }
    if (ON == 0) {
        showChar((char) 'O', pos1);
        showChar((char) 'F', pos2);
        showChar((char) 'F', pos3);
    } else {
        showChar((char) 'O', pos1);
        showChar((char) 'N', pos2);
    }
}

void display_dir() {
    dir_count = (dir_count + 1) % 8;
    display();
}

void run_status() {
    if (ON == 1) {
        ON = 0;
    } else {
        ON = 1;
    }
}

void turnMotorOnOrOff() {
    ON = 0;
    runStepper(0, 0, 0);
}

//#pragma vector=PORT1_VECTOR
//__interrupt void Port_1(void) {
//    GPIO_toggleOutputOnPin(GPIO_PORT_LED1, GPIO_PIN_LED1);
//    if (GPIO_getInterruptStatus (GPIO_PORT_S1, GPIO_PIN_S1) == GPIO_PIN_S1) { //BUTTON PRESSED
//        run_status();
//        motorHandler(1);
//        display();
//        GPIO_clearInterrupt(GPIO_PORT_S1, GPIO_PIN_S1);
//    }
////    else if (GPIO_getInterruptStatus (GPIO_PORT_S1, PI2_PIN) == PI2_PIN) {  //PHOTO INTERRUPTED
////        GPIO_clearInterrupt(GPIO_PORT_S1, PI2_PIN);
////    }
//}

//#pragma vector=PORT2_VECTOR
//__interrupt void Port_2(void) {
//    GPIO_toggleOutputOnPin(GPIO_PORT_LED2, GPIO_PIN_LED2);
//    display_dir();
////    __delay_cycles(100000);
//    GPIO_clearInterrupt(GPIO_PORT_S2, GPIO_PIN_S2);
//}


void motorHandler(int is_motor1, int isReversed, int isNormalSpeed) {
    if (ON == 0) { return; }
    display();
    while(1) {
        if ((readPI1() == 0) || ( readPI2() == 0)) {
            if (dir[dir_count/2] == 'U' || dir[dir_count/2] == 'D') {
                ON = 0;
                break;
            }
        }

        if ((readPI3() == 0) || ( readPI4() == 0)) {
            if (dir[dir_count/2] == 'R' || dir[dir_count/2] == 'L') {
                ON = 0;
                break;
            }
        }

        if (readBtn1() == 1) {
            ON = 0;
            break;
        }
        runStepper(is_motor1, isReversed, isNormalSpeed);
    }
    display();
}

void Init_GPIO(void) {
    // Set all GPIO pins to output low to prevent floating input and reduce power consumption
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P7, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN0|GPIO_PIN1|GPIO_PIN2|GPIO_PIN3|GPIO_PIN4|GPIO_PIN5|GPIO_PIN6|GPIO_PIN7);

    //    //Set LaunchPad switches as inputs - they are active low, meaning '1' until pressed
    //    GPIO_setAsInputPinWithPullUpResistor(SW1_PORT, SW1_PIN);
    //    GPIO_setAsInputPinWithPullUpResistor(SW2_PORT, SW2_PIN);
    //
    //    //Set LED1 and LED2 as outputs
    //    //GPIO_setAsOutputPin(LED1_PORT, LED1_PIN); //Comment if using UART
    //    GPIO_setAsOutputPin(LED2_PORT, LED2_PIN);
}


