#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#ifndef __AVR_ATmega328P__
/* just used for linting */
#include <avr/iom328p.h>
#endif
#include "StateMachine.h"
#include "StateMachine_cfg.h"

/* CP frequencz in MHz */
#ifndef F_CPU
#define F_CPU  16000000
#endif
/* UART baudrate */
#define USART_BAUDRATE 9600
/* calculated UBBR value */
#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* UART Tx */
int UART_Transmit(char character, FILE *stream)
{
    if (character == '\n')
    {
        UART_Transmit('\r', stream);
    }

	while (!(UCSR0A & (1 << UDRE0)))
    {
        ;
    }

	UDR0 = character;

	return 0;
}


/* prepare user-defined FILE buffer */
FILE mystdout = FDEV_SETUP_STREAM(UART_Transmit, NULL, _FDEV_SETUP_WRITE);


int main(void)
{
    /* UART initialization */
    UBRR0H = UBRR_VALUE >> 8;
    UBRR0L = UBRR_VALUE & 0xFF;

    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);

    /* stdout redirection */
    stdout = &mystdout;


    // RiceCookerSM example
    // RiceCooker_Cancel(&SM_riceCooker);
    RiceCooker_Start(&SM_riceCooker);
    while (RiceCooker_IsPollActive())
        RiceCooker_Poll(&SM_riceCooker);

    return 0;
}