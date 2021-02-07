#include <stdio.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <avr/interrupt.h>
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

int UART_Receive(FILE *stream)
{
    /* wait for data to be received */
    while (!(UCSR0A & (1 << RXC0)))
    {
        ;
    }

    /* get received data */
    return UDR0;
}

/* prepare user-defined FILE buffer */
FILE stream = FDEV_SETUP_STREAM(UART_Transmit, UART_Receive, _FDEV_SETUP_RW);

void ClearUserCommand(void)
{
    riceCookerStatus.userCommand = '\0';
}

/* UART Rx interrupt */
ISR(USART_RX_vect, ISR_BLOCK)
{
    /* recieve and write to Rx buffer */
    riceCookerStatus.userCommand = UART_Receive(&stream);

    switch (riceCookerStatus.userCommand)
    {
    case 's':
        SM_EventEmit(&SM_riceCooker, EVENT_START);
        ClearUserCommand();
        break;

    case 'c':
        SM_EventEmit(&SM_riceCooker, EVENT_CANCEL);
        ClearUserCommand();
        break;
    
    default:
        break;
    }
}

int main(void)
{
    /* UART initialization */
    cli();
    UBRR0H = UBRR_VALUE >> 8;
    UBRR0L = UBRR_VALUE & 0xFF;

    /* enable Rx, Tx, Rx Interrupt */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);

    /* stdout and stdin redirection */
    stdout = &stream;
    stdin  = &stream;
    sei();
  
    /* RiceCooker state machine example */
    while (1)
    {

        while (RiceCooker_IsPollActive())
        {
            SM_EventEmit(&SM_riceCooker, EVENT_POLL_COOK_STATUS);
        }
    }

    return 0;
}