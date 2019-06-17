#include "msp430F5529.h"
#include "Timer/timer.h"
#include "Button/button.h"
#include "UART/UART.h"

// Macros
#define LED_OUT P1OUT
#define LED_DIR P1DIR
#define G_LED BIT2
#define R_LED BIT3

#define MTR_OUT P1OUT
#define MTR_DIR P1DIR
#define MTR_SEL P1SEL
#define MTR_OP BIT4
#define MTR_CL BIT6

/* PINES
 *
 * LED
 * P1.2 <= Verde
 * P1.3 <= Rojo
 *
 * MOTOR
 * P1.4 <= Cerrador
 * P1.6 <= Abridor
 *
 * SENSORES
 * P2.0 <= Botón
 * P2.4 <= Abiero
 * P2.5 <= Cerrado
 */

/* ESTADOS
 *
 * 1. Cerrado
 * 2. Abriéndose check
 * 3. Abriéndose
 * 4. Abierto
 * 5. Cerrándose check
 * 6. Cerrándose
 *
 * 7. Detención Temporal
 * 8. Detención indefinida
 * 9. Error
 */

// Variables globales
unsigned short int state = 1;
unsigned short int state_change = 1;

// Struct global
struct USART_configuration config_9600_8N1 = {9600, 8, 'n', 1};

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    init_Debouncer();
    init_Sensor();
    init_UART(config_9600_8N1);

    // Debounce LED
    P4DIR |= BIT7;

    // LED y Motores como Output
    LED_DIR |= G_LED | R_LED;
    MTR_DIR |= MTR_OP | MTR_CL;

    // Habilita interrupciones
	_BIS_SR(GIE);

	while(1) {
	    if (state_change){
            state_change = 0;

            switch(state){

            case 1: // Cerrado
                LED_OUT &= ~(G_LED | R_LED);
                MTR_OUT &= ~(MTR_OP | MTR_CL);
                USART_Transmit_String("Cerrado");
                break;

            case 2: // Abriéndose Check
                LED_OUT &= ~R_LED;
                LED_OUT |= G_LED;
                MTR_OUT &= ~MTR_CL;
                MTR_OUT |= MTR_OP;
                USART_Transmit_String("Abr Check");

                // Habilita interrupción de sensor de apertura unicamente
                enable_OP_ISR();

                init_TimerA0(0.5); // Chequea 2 segundos
                reset_TimerA0();
                break;

            case 3: // Abriéndose
                LED_OUT &= ~R_LED;
                LED_OUT |= G_LED;
                MTR_OUT &= ~MTR_CL;
                MTR_OUT |= MTR_OP;
                USART_Transmit_String("Abriendose");

                // Habilita interrupción de sensor de apertura unicamente
                enable_OP_ISR();

                init_TimerA0(0.2); // Chequea 5 segundos
                reset_TimerA0();
                break;

            case 4: // Abierto
                LED_OUT &= ~(G_LED | R_LED);
                MTR_OUT &= ~(MTR_OP | MTR_CL);
                USART_Transmit_String("Abierto");
                break;


            case 5: //Cerrándose Check
                LED_OUT |= R_LED;
                LED_OUT &= ~G_LED;
                MTR_OUT |= MTR_CL;
                MTR_OUT &= ~MTR_OP;
                USART_Transmit_String("Cerrandose Check");

                // Habilita interrupción de sensor de cerrado unicamente
                enable_CL_ISR();

                init_TimerA0(0.5); // Chequea 2 segundos
                reset_TimerA0();
                break;

            case 6: //Cerrándose
                LED_OUT |= R_LED;
                LED_OUT &= ~G_LED;
                MTR_OUT |= MTR_CL;
                MTR_OUT &= ~MTR_OP;
                USART_Transmit_String("Cerrandose");

                // Habilita interrupción de sensor de cerrado unicamente
                enable_CL_ISR();

                init_TimerA0(0.2); // Chequea 5 segundos
                reset_TimerA0();
                break;

            case 7: // Detención temporal
                LED_OUT &= ~(G_LED | R_LED);
                MTR_OUT &= ~(MTR_OP | MTR_CL);
                USART_Transmit_String("Det. Temporal");


                init_TimerA0(0.1); // Chequea 10 segundos
                reset_TimerA0();
                break;

            case 8: // Detención indefinida
                LED_OUT |= (G_LED | R_LED);
                MTR_OUT &= ~(MTR_OP | MTR_CL);
                USART_Transmit_String("Det. Indef.");

                break;

            case 9: // Error
                LED_OUT &= ~(G_LED | R_LED);
                MTR_OUT &= ~(MTR_OP | MTR_CL);
                USART_Transmit_String("ERROR");


                // Habilita interrupción de sensor de cerrado unicamente
                enable_CL_ISR();

                init_TimerA0(4); // Blink cada segundo
                reset_TimerA0();
                break;
            }
	    }
	};
	return 0;
}


// ISR de Timer A0
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A0 (void)
{
    ISR_TimerA0(&state, &state_change);
}


// ISR de Puerto 2
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR (void)
{
    ISR_Port2(&state, &state_change);
}
