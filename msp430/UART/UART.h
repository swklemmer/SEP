#ifndef _USART_IMPLEMENT_ME_H_
#define _USART_IMPLEMENT_ME_H_

#include <stdint.h>

struct USART_configuration{
    unsigned long int baudio;
    int data_bits;
    char parity;
    int stop_bits;
    };

int init_UART(struct USART_configuration config);
void USART_Transmit_char(char data);
void USART_Transmit_String(char *string);


#endif // _USART_IMPLEMENT_ME_H_
