/*
 * LCD_V1.c
 *
 * Created: 03/05/2018 09:25:36
 * Author : Rodolfo Coutinho / Mateus Sousa
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

#include "LCD.h"
#include "DEF_PRINCIPAIS.h"
#include "USART.h"
#include "I2C.h"
#include "RTC.h"

#define RX_BUFFER_SIZE 1024
char UART_RX_Buffer[RX_BUFFER_SIZE];
char * UART_RX_ptr = UART_RX_Buffer;
char rx_task_flag = 0;
char * UART_TX_ptr;
char print_task_flag = 0xFF;
rtc_t rtc;

void setup_uc(void)
{
	/************IO PORT CONFIG************/
	//DDRX  - 0 = input | 1 = output
	//PORTX - if input = pull-up else port value
	
	//SET all PORTB as output
	DDRB = 0xFF;
	//SET PORTB initial value
	PORTB = 0x00;
	
	//SET all PORTC as output
	DDRC  = 0xFF;
	//PORTC initial value
	PORTC = 0x00;
	
	//SET all PORTD as output, except D0, D2, D3 and D4
	DDRD = 0xE2;
	//PORTD initial value
	PORTD = 0x1C;
	
	//Config INT0 and INT1 active on falling edge
	EICRA = 0b00001010;
	//INT0 and INT1 Interrupt MASK
	EIMSK = 0x03;
	
	//Enable Pin Change INT[2,1,0]
	PCICR = 0b00000100;
	
	/*Pin Change Interrupt MASK*/
	/*bits 23 to 16*/
	PCMSK2 = 0x10;
	/*bits 14 to 08*/	/*Doesn't exist PCINT15*/
	PCMSK1 = 0x00;
	/*bits 07 to 00*/
	PCMSK0 = 0x00;
	
	/************TIMER 1 CONFIG************/
	/* Generate 1s interrupt @ 16MHz      */
	/**************************************/
	
	TCCR1A = 0;                        // Check timer for normal operation pins at OC1A and OC1B
	TCCR1B = 0;                        // Clear register
	TCCR1B |= (1<<CS10)|(1 << CS12);   // Set up prescaler to 1024: CS12 = 1 e CS10 = 1
	
	TCNT1 = 0xC2F7;                    // Start timer with this value so that overflow occurs in 1 second
	// 65536-(16MHz/1024/1Hz) = 49911 = 0xC2F7
	
	TIMSK1 |= (1 << TOIE1);           // Enables TIMER1 to be interrupted
	
	
	/************TIMER 2 CONFIG************/
	/* Generate 2ms interrupt @ 16MHz     */
	/**************************************/
	
	//Timer/Counter Control Register A | COM2A1 COM2A0 COM2B1 COM2B0 -- WGM21 WGM20
	TCCR2A = 0x02;	//OC0A and OC0B disconnected; CTC MODE with OCR0A as TOP
	
	//Timer/Counter Control Register B | FOC2A FOC2B -- WGM22 CS22 CS21 CS20
	TCCR2B = 0x06;	//no force OC0A,B pins; prescale = 256
	
	//Timer/Counter Register
	TCNT2  = 0x00;
	//Output Compare Register A - in CTC mode with Prescale = 256 --> Fint = 500Hz | 2ms
	OCR2A  = 125;
	//Output Compare Register B
	OCR2B  = 0x00;
	//Timer/Counter Interrupt Mask Register | ----- OCIE2B OCIE2A TOIE2
	TIMSK2 = 0x02;	//Enable OCIE2A interrupt
}

//////////////////////////////////////////////////////////////////////////
//Functions
//////////////////////////////////////////////////////////////////////////


void USART_Transmit_String(char * m)
{
	/* Move MSG to UART_TX_PTR */
	UART_TX_ptr = m;
	
	/* Enable UART TX Interrupt */
	UCSR0B |= (1 << TXCIE0);
	
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	
	/* Put data into buffer, sends the data */
	UDR0 = *UART_TX_ptr;
}

//////////////////////////////////////////////////////////////////////////
//Global Variables
//////////////////////////////////////////////////////////////////////////

char Buttons_State = 0;

enum __App_States
{
	msg00_clear = 0x01,
	msg00_send  = 0x02,
	
	msg01_clear = 0x11,
	msg01_send  = 0x12,
	
	msg02_clear = 0x21,
	msg02_send  = 0x22,
	
	App_Iddle = 0x00
};
enum __App_States App_State = App_Iddle;


int i = 0;
int dec = 0;
int uni = 0;
char store_data[17];
char *store_data_ptr = store_data;
char store_time[15];
char *store_time_ptr = store_time;
char msg00[] = "LCD Test 1";
char *msg00_ptr = msg00;
char msg01[] = "New LCD Test";
char *msg01_ptr = msg01;
char msg02[] = "A B C and ...";
char *msg02_ptr = msg02;


//////////////////////////////////////////////////////////////////////////
//ISR's
//////////////////////////////////////////////////////////////////////////

ISR(TIMER1_OVF_vect)                             
{
	 
	RTC_GetDateTime(&rtc);
	
	switch(rtc.weekDay){
		case 1:
			store_data[0] = 'D';
			store_data[1] = 'O';
			store_data[2] = 'M';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 2:
			store_data[0] = 'S';
			store_data[1] = 'E';
			store_data[2] = 'G';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 3:
			store_data[0] = 'T';
			store_data[1] = 'E';
			store_data[2] = 'R';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 4:
			store_data[0] = 'Q';
			store_data[1] = 'U';
			store_data[2] = 'A';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 5:
			store_data[0] = 'Q';
			store_data[1] = 'U';
			store_data[2] = 'I';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 6:
			store_data[0] = 'S';
			store_data[1] = 'E';
			store_data[2] = 'X';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
		case 7:
			store_data[0] = 'S';
			store_data[1] = 'A';
			store_data[2] = 'B';
			store_data[3] = ' ';
			store_data[4] = '-';
		break;
	}
	
	char put_time[] = {'U', 'F', 'C', ' ', '-', ' ',((0xF0 & rtc.hour)>>4)+'0', (0x0F & rtc.hour)+'0', ':', ((0xF0 & rtc.min)>>4)+'0' , 
						(0x0F & rtc.min)+'0', ':', ((0xF0 & rtc.sec)>>4)+'0' , (0x0F & rtc.sec)+'0'};
	
	for(i = 0; i <= 14; i++) store_time[i] = put_time[i];
	
	//char put_date[] = {' ',((0xF0 & rtc.date)>>4)+'0', (0x0F & rtc.date)+'0', '/', ((0xF0 & rtc.month)>>4)+'0', (0x0F & rtc.month)+'0', '/', 
						//'2', '0', ((0xF0 & rtc.year)>>4)+'0', (0x0F & rtc.year)+'0'};
	
	store_data[5] = ' ';
	store_data[6] = ((0xF0 & rtc.date)>>4)+'0';
	store_data[7] = (0x0F & rtc.date)+'0';
	store_data[8] = '/';
	store_data[9] = ((0xF0 & rtc.month)>>4)+'0';
	store_data[10] = (0x0F & rtc.month)+'0';
	store_data[11] = '/';
	store_data[12] = '2';
	store_data[13] = '0';
	store_data[14] = ((0xF0 & rtc.year)>>4)+'0';
	store_data[15] = (0x0F & rtc.year)+'0';
	
	
	print_task_flag = 0xFF;

	TCNT1 = 0xC2F7;                                 // Restart TIMER to overflow in 1s
}

ISR(USART_TX_vect)
{
	UART_TX_ptr++;
	if (*UART_TX_ptr)
	{
		/* Put data into buffer, sends the data */
		UDR0 = *UART_TX_ptr;
	}
	else
	{
		/* Disable UART TX Interrupt at end of string*/
		UCSR0B &= ~(1 << TXCIE0);
	}
	
}

ISR(USART_RX_vect)
{
	char ch;
	
	//signalize errors
	PORTB = (UCSR0A & 0b00011100) >> 2;
	
	//read received byte
	ch = UDR0;

	//filter if its end of string (ENTER character)
	if (ch == '@')
	{
		*UART_RX_ptr = 0x00;
		UART_RX_ptr = UART_RX_Buffer;
		
		//warn main that a msg has been received
		rx_task_flag = 0xFA;
	}
	//else if (ch == 0x0A)
	else if (ch == 0x0D) //proteus only
	{
		*UART_RX_ptr = 0x00;
		UART_RX_ptr = UART_RX_Buffer;
		
		//warn main that a msg has been received
		rx_task_flag = 0xFF;
	}
	else
	{
		//Copy into buffer
		*UART_RX_ptr = ch;
		UART_RX_ptr++;
		//Roll over
		if (UART_RX_ptr >= (UART_RX_Buffer + RX_BUFFER_SIZE))
		{
			UART_RX_ptr = UART_RX_Buffer;
		}
	}
}

ISR(PCINT2_vect)	
{
	Buttons_State |= 0x04;	//button C
}

ISR(INT0_vect)		//button A
{
	rtc.hour = 0x00; //  00:00:00 am
	rtc.min =  0x00;
	rtc.sec =  0x00;

	rtc.date = 0x01; //1st Jan 2000
	rtc.month = 0x01;
	rtc.year = 0x00;
	rtc.weekDay = 1; // Sunday 
	
	RTC_SetDateTime(&rtc);
}

ISR(INT1_vect)		
{
	Buttons_State |= 0x02;	//button B
}

ISR(TIMER2_COMPA_vect)
{
	LCD_SM &= 0xFE;
}

//////////////////////////////////////////////////////////////////////////
// Application State Machines
//////////////////////////////////////////////////////////////////////////

void APP_State_Machine(void)
{
	if(print_task_flag == 0xFF){
		switch (App_State){
		
			case App_Iddle:
			App_State = msg00_clear;
			break;
		
			case msg00_clear:
			if (LCD_COMMAND(CLEAR_LCD)){
				App_State = msg00_send;
				LCD_POS_XY(1,1);
			}
			break;
		
			case msg00_send:
		
			if (LCD_DATA(*store_time_ptr)){
				store_time_ptr++;
				if (!*store_time_ptr){
					Buttons_State = 0x00;
					store_time_ptr = store_time;
					App_State = msg01_clear;
					//print_task_flag = 0x00;
				}
			}
		
		break;
		
			case msg01_clear:
			if (LCD_COMMAND(CLEAR_LCD)){
				App_State = msg01_send;
				LCD_POS_XY(1,2);
			}
			break;
			
			case msg01_send:
			
			if (LCD_DATA(*store_data_ptr)){
				store_data_ptr++;
				if (!*store_data_ptr){
					Buttons_State = 0x00;
					store_data_ptr = store_data;
					App_State = App_Iddle;
					print_task_flag = 0x00;
				}
			}
			
			break;
		
		
		default:
		break;
		} 
		} 
		
}

//////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////

int main(void)
{
	//Start Uart
	USART_init();
	
	//Start DSM-1307
	RTC_Init(0x00);
	
	//Start up uC
	setup_uc();
	
	//Turn on all interrupts
	sei();
	 
	//Hold and Wait LCD INIT
	//This make sure that LCD was correctly initialized
	LCD_INIT();
	
	//Do a LCD Test
	LCD_DATA('X');
	
	/*
	rtc.hour = 0x10; //  10:40:20 am
	rtc.min =  0x40;
	rtc.sec =  0x00;

	rtc.date = 0x01; //1st Jan 2016
	rtc.month = 0x01;
	rtc.year = 0x16;
	rtc.weekDay = 5; // Friday: 5th day of week considering monday as first day.
	
	RTC_SetDateTime(&rtc);

	*/
	
    /* Replace with your application code */
    while (1) 
    {
		
		if (rx_task_flag == 0xFF)
		{
			rx_task_flag = 0;
			//USART_Transmit_String(UART_RX_Buffer);
			USART_send('\r');
			
			rtc.hour = (UART_RX_Buffer[0]<<4 | UART_RX_Buffer[1]);
			
			rtc.min = ((UART_RX_Buffer[3]<<4) | UART_RX_Buffer[4]);
			
			rtc.sec = ((UART_RX_Buffer[6]<<4) | UART_RX_Buffer[7]);
			
			rtc.weekDay = 0x0F & UART_RX_Buffer[9];
			 
			rtc.date = ((UART_RX_Buffer[11]<<4) | UART_RX_Buffer[12]);
			
			rtc.month = ((UART_RX_Buffer[14]<<4) | UART_RX_Buffer[15]);
			
			rtc.year = ((UART_RX_Buffer[19]<<4) | UART_RX_Buffer[20]);
			
			
			if(UART_RX_Buffer[0] == '0'){
				rtc.hour &=~(1<<4);
				rtc.hour &=~(1<<5);
			}else{
				rtc.hour &=~(1<<5);
			}
			
			if(UART_RX_Buffer[0] == '2'){
				rtc.hour &=~(1<<4);
				rtc.hour |= (1<<5);
			}
			
			if(UART_RX_Buffer[3] == '0'){
				rtc.min &=~(1<<4);
				rtc.min &=~(1<<5);
			}else{
				rtc.min &=~(1<<5);
			}
			
			if(UART_RX_Buffer[3] == '2'){
				rtc.min &=~(1<<4);
				rtc.min |= (1<<5);
			}
			
			if(UART_RX_Buffer[3] == '3') rtc.min |= (1<<5);
			
			if(UART_RX_Buffer[3] == '4') rtc.min &=~(1<<4);
			
			if(UART_RX_Buffer[6] == '0'){
				rtc.sec &=~(1<<4);
				rtc.sec &=~(1<<5);
				}else{
				rtc.sec &=~(1<<5);
			}
			
			if(UART_RX_Buffer[6] == '2'){
				rtc.sec &=~(1<<4);
				rtc.sec |= (1<<5);
			}
			
			if(UART_RX_Buffer[6] == '3') rtc.sec |= (1<<5);
			
			if(UART_RX_Buffer[6] == '4') rtc.sec &=~(1<<4);
			
			if(UART_RX_Buffer[11] == '0'){
				rtc.date &=~(1<<4);
				rtc.date &=~(1<<5);
			}else{
				rtc.date &=~(1<<5);
			}
			
			if(UART_RX_Buffer[11] == '2'){
				rtc.date &=~(1<<4);
				rtc.date |= (1<<5);
			}
			
			if(UART_RX_Buffer[11] == '3') rtc.date |= (1<<5);
			
			if(UART_RX_Buffer[14] == '0'){
				rtc.month &=~(1<<4);
				rtc.month &=~(1<<5);
			}else{
				rtc.month &=~(1<<5);
			}
			
			if(UART_RX_Buffer[19] == '0'){
				rtc.year &=~(1<<4);
				rtc.year &=~(1<<5);
			}else{
				rtc.year &=~(1<<5);
			}
			
			if(UART_RX_Buffer[19] == '2'){
				rtc.year &=~(1<<4);
				rtc.year |= (1<<5);
			}
			
			if(UART_RX_Buffer[19] == '3') rtc.year |= (1<<5);
			
			 
			
			RTC_SetDateTime(&rtc);
			
		}
		else if (rx_task_flag == 0xFA)
		{
			rx_task_flag = 0;
			PORTC = ( ( (UART_RX_Buffer[1]-'0') << 1 ) + (UART_RX_Buffer[0]-'0') ) & 0x03;
		}
		else
		asm volatile("nop"::); 
		
		APP_State_Machine();
		//APP_State_Machine_V2();
		LCD_State_Machine();
		//Sleep Function should be called here
		//Wake-up on interrupt events
		
	}
}

