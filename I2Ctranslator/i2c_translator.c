/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \author
 *      Atmel Corporation: http://www.atmel.com \n
 *      Support email: avr@atmel.com
 *
 * $Revision: 2660 $
 * $Date: 2009-08-11 12:28:58 +0200 (ti, 11 aug 2009) $  \n
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/
#include "avr_compiler.h"
#include "twi_master_driver.h"
#include "twi_slave_driver.h"

/*! Defining the slave address. */
#define SLAVE_ADDRESS    0x55
/*! Defining number of bytes in buffer. */
#define NUM_BYTES        2

/*! CPU speed 30MHz, I2C master speed 100kHz and Baudrate Register Settings */
#define CPU_SPEED       30000000
#define BAUDRATE	100000
#define TWI_BAUDSETTING TWI_BAUD(CPU_SPEED, BAUDRATE)


/* Global variables */
TWI_Master_t twiMaster;    /*!< TWI master module. */
TWI_Slave_t twiSlave;      /*!< TWI slave module. */
uint8_t dst_reg;        /* destination reg addr */
uint8_t dst_addr;	/* destination device addr */
uint8_t dst_data;       /* destination reg data to be written */
uint8_t readData;       /* Data read from the DAC registor */
uint16_t encrypted_reg;
/*! Buffer with data to be sent to the DAC.*/
uint8_t sendBuffer[NUM_BYTES] = {0x00, 0X00};


/*! This function perform the read/write operation from/to the DAC respectively
 *  and report back to RPi if necessary. 
 */
bool TWIC_SlaveProcessData(void)
{
	bool result = true;
	//To read a register value, we need to write the register 
	//address to DAC and then read back.
  	if(twiSlave.bytesReceived == 2) {

	  	sendBuffer[0] = dst_reg;
		//Wait until the master is ready
		while (twiMaster.status != TWIM_STATUS_READY)
		{
		}
		TWI_MasterWriteRead(&twiMaster,
		                    dst_addr,
		                    &sendBuffer[0],
		                    1,
		                    1);
		if(twiMaster.status != TWIM_RESULT_OK)
		{

			result = false;
			twiMaster.interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
			twiMaster.status = TWIM_STATUS_READY;
		}
  	}
	//To write a register value, we need to write the register 
	//address followed by the value to be written, no need to read back.
	else if(twiSlave.bytesReceived == 3) 
        {
	  	sendBuffer[0] = dst_reg;
	  	sendBuffer[1] = dst_data;
		while (twiMaster.status != TWIM_STATUS_READY)
		{
		}
		TWI_MasterWriteRead(&twiMaster,
		                    0x48,
		                    &sendBuffer[0],
		                    2,
		                    0);
		if(twiMaster.status != TWIM_RESULT_OK)
		{
			result = false;
		  	twiMaster.interface->MASTER.CTRLC = TWI_MASTER_CMD_STOP_gc;
			twiMaster.status = TWIM_STATUS_READY;
		}
	}
        else {
                return false;
        }
	return result;
}


/*! /brief Main function
 *
 *  The main function that setup the needed interrupt vectors
 */
int main(void)
{
  	encrypted_reg = 0;
	/* Initialize TWI master on port E with medium priority. */
	TWI_MasterInit(&twiMaster,
	               &TWIE,
	               TWI_MASTER_INTLVL_MED_gc,
	               TWI_BAUDSETTING);
	/* Initialize TWI slave on port C with low priority. */
	TWI_SlaveInitializeDriver(&twiSlave, &TWIC, TWIC_SlaveProcessData);
	TWI_SlaveInitializeModule(&twiSlave,
	                          SLAVE_ADDRESS,
	                          TWI_SLAVE_INTLVL_LO_gc);
	/* Enable LO/MID/HI interrupt level. */
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	PMIC.CTRL |= PMIC_HILVLEX_bm;
	PMIC.CTRL |= PMIC_MEDLVLEX_bm;
	sei();
	//Wait for the interrupt
	while (1) {
	}

}

/*! TWIC Master Interrupt vector. */
ISR(TWIE_TWIM_vect)
{
	TWI_MasterInterruptHandler(&twiMaster);
	readData = twiMaster.readData[0];
}

/*! TWIC Slave Interrupt vector. */
ISR(TWIC_TWIS_vect)
{
	TWI_SlaveInterruptHandler(&twiSlave);

}