/*
 * USART.h
 *
 * Created: 20/05/2018 17:19:12
 *  Author: Mateus Sousa
 */ 


#ifndef USART_H_
#define USART_H_

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

void USART_init(void);
void USART_send(unsigned char data);
unsigned char USART_Receive(void);
void USART_putstring(char* StringPtr);


#endif /* USART_H_ */