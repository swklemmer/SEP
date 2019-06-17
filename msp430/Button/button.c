#include "msp430F5529.h"
#include "button.h"

#define BTN_IN P2IN
#define BTN_OUT P2OUT
#define BTN_REN P2REN
#define BTN_DIR P2DIR
#define BTN_IE P2IE
#define BTN_IES P2IES
#define BTN_IFG P2IFG
#define BTN BIT0

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

#define DEB_TIME 4000


void init_Debouncer(void){
    // Inicializa botón como input pullup
    BTN_DIR &= ~BTN;
    BTN_REN |= BTN;
    BTN_OUT |= BTN;
    // Habilita interrupción de PinChange
    BTN_IE |= BTN;
    // Detecta flancos negativos
    BTN_IES |= BTN;


    // Setup de Timer: TASSEL_1: clk = 32kHz, MC_1: cuenta arriba, ID_3: divide clk en 8
    TA2CTL = TASSEL_1 | MC_1 | ID_3;
    TA2CCR0 = DEB_TIME;
    // Deshabilita interrupcion de Timer
    TA2CCTL0 &= ~CCIE;
}

void init_Sensor(void) {
    // Sensores como input pullup con interrupción en flanco neg.
    SNS_DIR &= ~(SNS_OP | SNS_CL);
    SNS_SEL &= ~(SNS_OP | SNS_CL);

    SNS_REN |= (SNS_OP | SNS_CL);
    SNS_OUT |= (SNS_OP | SNS_CL);

    SNS_IES |= (SNS_CL | SNS_OP);
}

void enable_OP_ISR(void) {
    // Habilita interrupción de sensor de apertura unicamente
    SNS_IE |= SNS_OP;
    SNS_IE &= ~SNS_CL;
}

void enable_CL_ISR(void) {
    // Habilita interrupción de sensor de cerrado unicamente
    SNS_IE &= ~SNS_OP;
    SNS_IE |= SNS_CL;
    SNS_IFG &= SNS_CL;
}

// ISR de PinChange 2
void ISR_Port2(unsigned short int *state, unsigned short int *state_change) {

    switch(P2IV) {
    case 0x02: // Botón
        // Deshabilita interrupción de PinChange
        BTN_IE &= ~BTN;

        // Resetea timer y habilita interrupción
        TA2R = 0;
        TA2CCTL0 = CCIE;

        P4OUT |= BIT7;
        switch(*state){
            case 1: // Cerrado
                *state = 2;
                break;
            case 2: //Abriéndose Check
                *state = 7;
                break;
            case 3: // Abriéndose
                *state = 7;
                break;
            case 4: // Abierto
                *state = 5;
                break;
            case 5: // Cerrándose Check
                *state = 7;
                break;
            case 6: // Cerrándose
                *state = 7;
                break;

            case 7: // Detención temporal
                *state = 8;
                break;
            case 8: // Detención indefinida
                *state = 6;
                break;
            case 9: // Error
                *state = 6;
                break;
            }
        *state_change = 1;
        break;

    case 0x0A: // Cerrado
        if ((SNS_IN & SNS_CL) == 0x00) {

            // Deshabilita interrupción de sensor de cerrad
            SNS_IE &= ~SNS_CL;
            SNS_IES &= ~SNS_CL;

            switch(*state){
                case 5: //Cerrándose Check
                    *state = 1;
                    break;
                case 6: // Cerrándose
                    *state = 1;
                    break;
                case 9: // Error
                    *state = 1;
                    break;
                default:
                    *state = 9;
                    break;
                }
            *state_change = 1;
        }
        break;
    case 0x0C: // Abierto
        if ((SNS_IN & SNS_OP) == 0x00) {

            // Deshabilita interrupción de sensor de apertura
            SNS_IE &= ~SNS_OP;
            SNS_IES &= ~SNS_OP;

            switch(*state){
                case 2: //Abriéndose Check
                    *state = 4;
                    break;
                case 3: // Abriéndose
                    *state = 4;
                    break;
                default:
                    *state = 9;
                    break;
                }
            *state_change = 1;
        }
        break;
    }
}



// ISR de Timer A2
#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2 (void)
{
    // Deshabilita interrupcion de Timer
    TA2CCTL0 &= ~CCIE;

    // Habilita interrupción de PinChange y limpia flag
    BTN_IE |= BTN;
    BTN_IFG = 0x00;

    // Has algo aquí
    P4OUT &= ~BIT7;

}
