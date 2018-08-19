/*
 * LCD.c
 *
 * Created: 09/05/2018 10:57:06
 *  Author: Rodolfo Coutinho / Mateus Sousa
  * OBS: LIB optimized to use only 4 bit data size
 */ 

#include "LCD.h"
#include <util/atomic.h>

//global variables
enum LCD_States	LCD_SM = LCD_IDLE;

//private variables
char LCD_CMD = 0;	//Hold TEMP COMMAND	
char LCD_D = 0;		//Hold TEMP DATA

void LCD_INIT(void)
{
	//Do initialization sequence
	LCD_SM = _init_1;
	do 
	{
		LCD_State_Machine();
	} while (LCD_SM != LCD_IDLE);

	//Default Operation MODE: 4bits (0x28) | CURSOR_OFF (0x0C) | CURSOR_INS_RIGHT (0x06) | CLEAR_LCD (0x01)
	LCD_SM = LCD_CwriteH;
	LCD_CMD = 0x28;
	do
	{
		LCD_State_Machine();
	} while (LCD_SM != LCD_IDLE);
	
	LCD_SM = LCD_CwriteH;
	LCD_CMD = 0x0C;
	do
	{
		LCD_State_Machine();
	} while (LCD_SM != LCD_IDLE);
	
	LCD_SM = LCD_CwriteH;
	LCD_CMD = 0x06;
	do
	{
		LCD_State_Machine();
	} while (LCD_SM != LCD_IDLE);
	
	LCD_SM = LCD_CwriteH;
	LCD_CMD = 0x01;
	do
	{
		LCD_State_Machine();
	} while (LCD_SM != LCD_IDLE);
}
char LCD_COMMAND(char CMD)
{
	enum LCD_States TEMP;
	char ret_val = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TEMP = LCD_SM;
		if (TEMP == LCD_IDLE)
		{
			LCD_SM = LCD_CwriteH;
			LCD_CMD = CMD;
			ret_val = 1;
		} 
		else
		{
			ret_val = 0;
		}
	}
	return ret_val;
}
char LCD_DATA(char D)
{
	enum LCD_States TEMP;
	char ret_val = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		TEMP = LCD_SM;
		if (TEMP == LCD_IDLE)
		{
			LCD_SM = LCD_DwriteH;
			LCD_D = D;
			ret_val = 1;
		}
		else
		{
			ret_val = 0;
		}
	}
	return ret_val;
}
void LCD_POS_XY(char X, char Y)
{
	unsigned int endereco = 0;

	if(Y==1)
		endereco = lcd_L1;
	else if(Y==2)
		endereco = lcd_L2;
	else if(Y==3)
		endereco = lcd_L3;
	else if(Y==4)
		endereco = lcd_L4;

	endereco += X-1;
	
	LCD_SM = LCD_CwriteH;
	LCD_CMD = endereco;
}

void LCD_State_Machine(void)
{
	//if not on WAIT STATE
	if (!(LCD_SM & 0x01))
	{
		switch (LCD_SM)
		{
			//////////////////////////////////////////////////////////////////////////
			// LCD_IDLE State
			case LCD_IDLE:
				LCD_D_PORT = 0x00;
				break;

			//////////////////////////////////////////////////////////////////////////
			// LCD_init sequence
			case _init_1:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = 0x03 + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
			
				//Set Next State plus wait states
				LCD_SM = _init_2 + 1;
				break;
			case _init_2:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = 0x03 + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
			
				//Set Next State plus wait states
				LCD_SM = _init_3 + 1;
				break;
			case _init_3:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = 0x03 + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
			
				//Set Next State plus wait states
				LCD_SM = _init_4 + 1;
				break;
			case _init_4:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = 0x02 + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
			
				//Set Next State plus wait states
				LCD_SM = LCD_IDLE + 1;
				break;
		
			//////////////////////////////////////////////////////////////////////////
			// Generic LCD write COMMAND 
			case LCD_CwriteH: 
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = (LCD_CMD >> 4) + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
				
				//Set Next State plus wait states
				LCD_SM = LCD_CwriteL + 1;
				break;
			case LCD_CwriteL:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = (LCD_CMD & 0x0F) + LCD_EN;
				LCD_D_PORT ^= LCD_EN;
				
				//Set Next State plus wait states
				LCD_SM = LCD_IDLE + 1;
				break;
			
			//////////////////////////////////////////////////////////////////////////
			// Generic LCD write DATA 
			case LCD_DwriteH:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = (LCD_D >> 4) + LCD_EN + LCD_RS;
				LCD_D_PORT ^= LCD_EN;
				
				//Set Next State plus wait states
				LCD_SM = LCD_DwriteL + 1;
				break;
			case LCD_DwriteL:
				//Send Nibble
				LCD_D_PORT = 0x00;
				LCD_D_PORT = (LCD_D & 0x0F) + LCD_EN + LCD_RS;
				LCD_D_PORT ^= LCD_EN;
				
				//Set Next State plus wait states
				LCD_SM = LCD_IDLE + 1;
				break;
		}
	}
}
