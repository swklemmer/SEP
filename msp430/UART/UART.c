// LIBRERÍAS Y CONSTANTES
#include <msp430F5529.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "UART.h"

#define F_SMCLK  1048576


// DECLARACIONES DE FUNCIONES

int init_UART(struct USART_configuration config)
{
    // Detiene el WatchDog
    WDTCTL = WDTPW | WDTHOLD;

    // Enciende pines de Transmisión y Recepción
    P3SEL |= BIT3 + BIT4;

    // Ponemos el estado en reset para configurar
    UCA0CTL1 |= UCSWRST;

    // Clock para comunicación es SMCLK (1 048 576 Hz)
    UCA0CTL1 |= UCSSEL_2;

    // Configura baudio
    float n = F_SMCLK / config.baudio;
    int ucbr = n;
    int ucbrs = round((n - ucbr) * 8);

    UCA0BR0 = ucbr;
    UCA0BR1 = (ucbr >> 8);
    UCA0MCTL = (ucbrs << 1) + UCBRF_0;

    // Configura data bits
    switch (config.data_bits){
        case 7:
            UCA0CTLW0 |= UC7BIT;
            break;

        case 8:
            UCA0CTLW0 &= ~UC7BIT;
            break;

        default:
            printf("ERROR: La configuración de 'data bits' es incorrecta.\n");
            return 1;
    }

    // Configura Paridad
    switch (config.parity){
        case 'n':
            UCA0CTL0 &= ~UCPEN;
            break;

        case 'e':
            UCA0CTLW0 |= UCPEN;
            UCA0CTLW0 |= UCPAR;
            break;

        case 'o':
            UCA0CTLW0 |= UCPEN;
            UCA0CTLW0 &= ~UCPAR;
            break;

        default:
            printf("ERROR: La configuración de 'paridad' es incorrecta.\n");
            return 2;
        }

    // Configura Stop Bits
    switch (config.stop_bits){
        case 1:
            UCA0CTLW0 &= ~UCSPB;
            break;

        case 2:
            UCA0CTLW0 |= UCSPB;
            break;

        default:
            printf("ERROR: La configuración de 'stop bits' es incorrecta.\n");
            return 3;
    }

    // Volvemos al estado de uso
    UCA0CTL1 &= ~UCSWRST;
    _delay_cycles(1000000);

    return 0;
}



// Transmite solo un char, data corresponde al char a enviar
void USART_Transmit_char(char data)
{
    while (!(UCA0IFG & UCTXIFG));
    UCA0TXBUF = (char) data;
}



// Transmite un string completo
void USART_Transmit_String(char *string)
{
    char *str_pointer;
    str_pointer = string;
    while (*str_pointer != '\0'){
        char character = *str_pointer;
        USART_Transmit_char(character);
        *str_pointer++;
    }

}
