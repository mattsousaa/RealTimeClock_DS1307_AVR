/*
 * USART.c
 *
 * Created: 20/05/2018 17:20:16
 *  Author: Mateus Sousa
 */ 

#include "USART.h"
#include "DEF_PRINCIPAIS.h"

void USART_init(void){
	
	/************************************************************************/
	/*UART			                                                        */
	/************************************************************************/
	
	/*Set baud rate */
	UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1 << RXCIE0)|(1 << TXCIE0);
	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}
//------------------------------------------------------------------------------------
void USART_send(unsigned char data){
	/* Wait for empty transmit buffer */
	while(!(UCSR0A & (1<<UDRE0)));
	
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

//------------------------------------------------------------------------------------
unsigned char USART_Receive(void){
	/* Wait for data to be received */
	while (!(UCSR0A & (1<<RXC0)));
	/* Get and return received data from buffer */
	return UDR0;
}
//------------------------------------------------------------------------------------
void USART_putstring(char* StringPtr){
	// sends the characters from the string one at a time to the USART
	while(*StringPtr != 0x00)
	{
		USART_send(*StringPtr);
		StringPtr++;
	}
}