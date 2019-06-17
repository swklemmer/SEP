#include "msp430F5529.h"
#include "timer.h"

// Macros
#define LED_OUT P1OUT
#define LED_DIR P1DIR
#define G_LED BIT2
#define R_LED BIT3

#define SNS_IN P2IN
#define SNS_OUT P2OUT
#define SNS_DIR P2DIR
#define SNS_REN P2REN
#define SNS_SEL P2SEL
#define SNS_IE P2IE
#define SNS_IES P2IES
#define SNS_IFG P2IFG
#define SNS_OP BIT5
#define SNS_CL BIT4


// Timer A0

void init_TimerA0(float frecuency) {
    // Deshabilita interrupción de Timer A0
    TA0CCTL0 &= ~CCIE;
    // TASSEL_1: clk = 32kHz, MC_1: up-count, ID_3: divide clk en 8
    TA0CTL = TASSEL_1 | MC_1 | ID_3;
    // Registro de comparación
    TA0CCR0 = 4000/frecuency;
}

void stop_TimerA0(void){
    // Deshabilita interrupcion de Timer
    TA0CCTL0 &= ~CCIE;
}

void reset_TimerA0(void){
    // Resetea timer y habilita interrupción
    TA0R = 0;
    TA0CCTL0 |= CCIE;
}


// Timer A1

void init_TimerA1(float frecuency) {
    // Deshabilita interrupción de Timer A1
    TA1CCTL0 &= ~CCIE;
    // TASSEL_1: clk = 32kHz, MC_1: up-count, ID_3: divide clk en 8
    TA1CTL = TASSEL_1 | MC_1 | ID_3;
    // Registro de comparación
    TA1CCR0 = 4000/frecuency;
}


void reset_TimerA1(void){
    // Resetea timer y habilita interrupción
    TA1R = 0;
    TA1CCTL0 = CCIE;
}

void stop_TimerA1(void){
    // Deshabilita interrupcion de Timer
    TA1CCTL0 &= ~CCIE;
}

// ISR de Timer A0
void ISR_TimerA0(unsigned short int *state, unsigned short int *state_change) {

    switch(*state){
        case 1: // Cerrado
            break;
        case 2: // Abriéndose check
            if ((SNS_IN & (SNS_OP | SNS_CL)) == (SNS_OP | SNS_CL)) {
                *state = 3;
            }
            else {
                *state = 9;
            }

            *state_change = 1;
            stop_TimerA0();
            break;
        case 3: // Abriéndose
            if ((SNS_IN & SNS_OP) == 0x00) {
                *state = 4;
            }
            else {
                *state = 9;
            }

            *state_change = 1;
            stop_TimerA0();
            break;
        case 4: // Abierto
            break;
        case 5: //Cerrándose Check
            if ((SNS_IN & (SNS_OP | SNS_CL)) == (SNS_OP | SNS_CL)) {
                *state = 6;
            }
            else {
                *state = 9;
            }

            *state_change = 1;
            stop_TimerA0();
            break;
        case 6: // Cerrándose
            if ((SNS_IN & SNS_CL) == 0x00) {
                *state = 1;
            }
            else {
                *state = 9;
            }

            *state_change = 1;
            stop_TimerA0();
            break;

        case 7: // Det. temporal
            *state = 6;
            *state_change = 1;
            break;
        case 8: // Det. indef.
            break;
        case 9: // Error
            LED_OUT ^= G_LED;
            LED_OUT ^= R_LED;
            break;
        }
}


// ISR de Timer A1
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1 (void)
{
    // Haz algo aquí
    stop_TimerA1();
}

