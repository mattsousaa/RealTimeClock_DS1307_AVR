/*
 * LCD.h
 *
 * Created: 20/05/2018 17:00:52
 *  Author: Rodolfo Coutinho / Mateus Sousa
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <avr/io.h>

//////////////////////////////////////////////////////////////////////////
// LCD interface pins
//////////////////////////////////////////////////////////////////////////

//LCD Data PORT
#define LCD_D_PORT	PORTB

//LCD Control PORT
#define LCD_C_PORT	PORTB

#define LCD_EN	0b00100000
#define LCD_RS	0b00010000

//////////////////////////////////////////////////////////////////////////
// LCD Commands
//////////////////////////////////////////////////////////////////////////

	#define CLEAR_LCD			0x01
   
	#define CURSOR_HOME			0x02
	#define CURSOR_ON			0x0E
	#define CURSOR_OFF			0x0C
	#define CURSOR_PISCA		0x0D
	#define CURSOR_ALT			0x0F
	
	#define CURSOR_MOV_LEFT		0x10
	#define CURSOR_MOV_RIGHT	0x14
	#define CURSOR_INS_LEFT		0x04
	#define CURSOR_INS_RIGHT	0x06
	
	#define MSG_MOV_LEFT		0x18
	#define MSG_MOV_RIGHT		0x1C
	#define MSG_INS_LEFT		0x07
	#define MSG_INS_RIGHT		0x05
	
//////////////////////////////////////////////////////////////////////////   
//LCD size definitions
//////////////////////////////////////////////////////////////////////////

   #define LCD_COL 1		/*number of columns D: 1 --> 16 columns
                                                   0 --> 20 columns*/
   //Line address definitions

   #define lcd_L1	0x80 // 1st line address on LCD RAM
   #define lcd_L2	0xC0 // 2nd line address on LCD RAM
   
   #if LCD_COL
		#define lcd_L3	0x90	// 3rd line address on LCD RAM
		#define lcd_L4  0xD0	// 4th line address on LCD RAM
   #else
   		#define lcd_L3	0x94	// 3rd line address on LCD RAM
   		#define lcd_L4  0xD4	// 4th line address on LCD RAM
   #endif
   
//////////////////////////////////////////////////////////////////////////
// LCD Functions and parameters
//////////////////////////////////////////////////////////////////////////

extern void LCD_INIT(void);
extern char LCD_COMMAND(char CMD);
extern char LCD_DATA(char D);
extern void LCD_POS_XY(char X, char Y);

extern void LCD_State_Machine(void);
enum   LCD_States
{
	//Initialization Sequence
	_init_1 = 0x10,
	_init_2 = 0x20,
	_init_3 = 0x30,
	_init_4 = 0x40,
	
	LCD_IDLE	= 0x00,
	
	LCD_CwriteH = 0xE0,
	LCD_CwriteL = 0xE8,
	
	LCD_DwriteH = 0xF0,
	LCD_DwriteL = 0xF8
};
extern enum LCD_States	LCD_SM;



#endif /* LCD_H_ */