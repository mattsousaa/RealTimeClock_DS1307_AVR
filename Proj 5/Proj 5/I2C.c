/*
 * I2C.c
 *
 * Created: 20/05/2018 18:17:59
 *  Author: Mateus Sousa
 */ 

#include <avr/io.h>
#include "I2C.h"

/***************************************************************************************************
                         void I2C_Init()
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value	: none

 * description :This function is used to initialize the I2C module
------------------------------------------------------------------------------------*/
void I2C_Init()
{
    TWSR=0x00; //set presca1er bits to zero
    TWBR=0x46; //SCL frequency is 50K for 16Mhz
    TWCR=0x04; //enab1e TWI module
}


/***************************************************************************************************
                         void I2C_Start()
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate I2C Start Condition.
                 Start Condition: SDA goes low when SCL is High.

                               ____________
                SCL:          |            |
                      ________|            |______
                           _________
                SDA:      |         |
                      ____|         |____________

 ***************************************************************************************************/
void I2C_Start()
{
    TWCR = ((1<<TWINT) | (1<<TWSTA) | (1<<TWEN));
    while (!(TWCR & (1<<TWINT)));
}


/***************************************************************************************************
                         void I2C_Stop()
 ****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : none

 * description  :This function is used to generate I2C Stop Condition.
                 Stop Condition: SDA goes High when SCL is High.

                               ____________
                SCL:          |            |
                      ________|            |______
                                 _________________
                SDA:            |
                      __________|

 ***************************************************************************************************/

void I2C_Stop(void)
{
    TWCR = ((1<< TWINT) | (1<<TWEN) | (1<<TWSTO));
    //DELAY_us(100) ; //wait for a short time
}

/***************************************************************************************************
                         void I2C_Write(uint8_t v_i2cData_u8)
 ****************************************************************************************************
 * I/P Arguments: uint8_t-->8bit data to be sent.
 * Return value  : none

 * description  :This function is used to send a byte on SDA line using I2C protocol
                 8bit data is sent bit-by-bit on each clock cycle.
                 MSB(bit) is sent first and LSB(bit) is sent at last.
                 Data is sent when SCL is low.

         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
 SCL:   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___

 SDA:    D8       D7     D6      D5      D4       D3      D2      D1      D0     ACK


 ***************************************************************************************************/
void I2C_Write(uint8_t v_i2cData_u8)
{
    TWDR = v_i2cData_u8 ;
    TWCR = ((1<< TWINT) | (1<<TWEN));
    while (!(TWCR & (1 <<TWINT)));
}

/***************************************************************************************************
                         uint8_t I2C_Read(uint8_t v_ackOption_u8)
****************************************************************************************************
 * I/P Arguments: none.
 * Return value  : uint8_t(received byte)

 * description :This fun is used to receive a byte on SDA line using I2C protocol.
               8bit data is received bit-by-bit each clock and finally packed into Byte.
               MSB(bit) is received first and LSB(bit) is received at last.


         ___     ___     ___     ___     ___     ___     ___     ___     ___     ___
SCL:    |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
      __|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |__

 SDA:    D8       D7     D6      D5       D4     D3       D2      D1     D0      ACK


***************************************************************************************************/
uint8_t I2C_Read(uint8_t v_ackOption_u8)
{
    TWCR = ((1<< TWINT) | (1<<TWEN) | (v_ackOption_u8<<TWEA));
    while ( !(TWCR & (1 <<TWINT)));
    return TWDR;
}