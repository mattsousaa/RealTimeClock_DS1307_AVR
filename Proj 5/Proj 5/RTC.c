/*
 * RTC.c
 *
 * Created: 20/05/2018 17:02:30
 *  Author: Mateus Sousa
 */ 

#include <avr/io.h>
#include "RTC.h"
#include "I2C.h"

//////////////////////////////////////////////////////////////////////////
//RTC_Init
//////////////////////////////////////////////////////////////////////////
//ARG: control register value
//RET: void
//DES: This function is used to Initialize the Ds1307 RTC
//OBS: Uncomment I2C_Init function if DS1307 is the only CI in I2C bus
//////////////////////////////////////////////////////////////////////////
void RTC_Init(char CtrlREG)
{
	//I2C_Init();		// Initialize the I2C module - removed to avoid reduntant call
	// IF DS1307 is only I2C module in bus, Uncomment
	
	//----------------------------------------------------------------------//
	
	I2C_Start();                            // Start I2C communication

	I2C_Write(Ds1307WriteMode);				// Connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(Ds1307ControlRegAddress);		// Select the Ds1307 ControlRegister to configure Ds1307

	I2C_Write(CtrlREG);                     // Write CtrlREG

	I2C_Stop();                             // Stop I2C communication after initializing DS1307
}

//////////////////////////////////////////////////////////////////////////
//RTC_SetDateTime
//////////////////////////////////////////////////////////////////////////
//ARG: Pointer to structure of type __RTC
//RET: void
//DES: This function is used to update the date info of Ds1307 RTC
//OBS: DS1307 date registers are all in BCD8421 format
//////////////////////////////////////////////////////////////////////////
void RTC_SetDateTime(rtc_t *rtc)
{
	I2C_Start();							// Start I2C communication

	I2C_Write(Ds1307WriteMode);				// connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(Ds1307SecondRegAddress);		// Request sec RAM address at 00H

	I2C_Write(rtc->sec);                    // Write sec from RAM address 00H
	I2C_Write(rtc->min);                    // Write min from RAM address 01H
	I2C_Write(rtc->hour);                   // Write hour from RAM address 02H
	I2C_Write(rtc->weekDay);                // Write weekDay on RAM address 03H
	I2C_Write(rtc->date);                   // Write date on RAM address 04H
	I2C_Write(rtc->month);                  // Write month on RAM address 05H
	I2C_Write(rtc->year);                   // Write year on RAM address 06h

	I2C_Stop();                             // Stop I2C communication after Setting the Date
}

//////////////////////////////////////////////////////////////////////////
//RTC_GetDateTime
//////////////////////////////////////////////////////////////////////////
//ARG: Pointer to structure of type __RTC
//RET: void
//DES: This function is used to read date info from DS1307 RTC
//OBS: DS1307 date registers are all in BCD8421 format
//////////////////////////////////////////////////////////////////////////
void RTC_GetDateTime(rtc_t *rtc)
{
	I2C_Start();							// Start I2C communication

	I2C_Write(Ds1307WriteMode);				// connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(Ds1307SecondRegAddress);		// Request Sec RAM address at 00H

	I2C_Stop();								// Stop I2C communication after selecting Sec Register

	I2C_Start();                            // Start I2C communication
	I2C_Write(Ds1307ReadMode);				// connect to DS1307(Read mode) by sending its ID

	rtc->sec = I2C_Read(1);					// read second and return Positive ACK
	rtc->min = I2C_Read(1);                 // read minute and return Positive ACK
	rtc->hour= I2C_Read(1);					// read hour and return Negative/No ACK
	rtc->weekDay = I2C_Read(1);				// read weekDay and return Positive ACK
	rtc->date= I2C_Read(1);					// read Date and return Positive ACK
	rtc->month=I2C_Read(1);					// read Month and return Positive ACK
	rtc->year =I2C_Read(0);					// read Year and return Negative/No ACK

	I2C_Stop();                              // Stop I2C communication after reading the Date
}