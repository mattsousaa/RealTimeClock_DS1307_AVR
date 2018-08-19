/*
 * RTC.h
 *
 * Created: 20/05/2018 16:59:42
 *  Author: Mateus Sousa
 */ 


#ifndef RTC_H_
#define RTC_H_

#define Ds1307ReadMode   0xD1  // DS1307 ID plus read mode
#define Ds1307WriteMode  0xD0  // DS1307 ID plus write mode

#define Ds1307SecondRegAddress   0x00   // Address to access Ds1307 SEC register
#define Ds1307DateRegAddress     0x04   // Address to access Ds1307 DATE register
#define Ds1307ControlRegAddress  0x07   // Address to access Ds1307 CONTROL register

//////////////////////////////////////////////////////////////////////////
//Real time clock struct
//Second | Minute | Hour | Day of Week | Day of Month | Month | Year
//////////////////////////////////////////////////////////////////////////
typedef struct
{
	char sec;
	char min;
	char hour;
	char weekDay;
	char date;
	char month;
	char year;
}rtc_t;

void RTC_Init(char CtrlREG);
void RTC_SetDateTime(rtc_t *rtc);
void RTC_GetDateTime(rtc_t *rtc);

#endif /* RTC_H_ */