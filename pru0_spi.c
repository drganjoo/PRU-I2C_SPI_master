/*
 *Written by Vaibhav Choudhary under GSOC-2016 for BeagleBoard.org
 *Copyright (C) 2016 Vaibhav Choudhary -www.vaibhavchoudhary.com
 *
 *This code may be copied and/or modified freely according to GNU General Public  
 *License version 2 as published by the Free Software Foundation, provided   
 *the following conditions are also met:
 *1) Redistributions/adaptions of source code must retain this copyright
 *   notice on the top, giving credit to the original author, along with 
 *   this list of conditions.
 *
 *2) Redistributions in binary form, compiled from this source code and/or 
 *   modified/adapted versions of this source code, must include this copyright 
 *   notice giving credit to the original author, along with this list of conditions 
 *   in the documentation and other materials provided with the
 *   distribution.
 *
 *3) The original author shall not held for any loss arising from using this code.
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 */
#include <stdint.h>
#include "resource_table_empty.h"
#include <pru_cfg.h>
#define   MOSI  15		//P8_11
#define   CLK  14		//P8_12
#define 	MISO 5		//P9_27
#define 	CS 3		//P9_28
volatile register uint32_t __R30;
volatile register uint32_t __R31;
uint8_t CPOL = 0, CPHA = 0;
uint8_t i = 0;

void main()
{
	//Declare  pointers to the mem location in the shared mem
	volatile uint8_t *mosi_transfer = (volatile uint8_t *)0x00010000;
	volatile uint8_t *miso_transfer = (volatile uint8_t *)(0x00010000 + 8);
	volatile uint8_t *mosi_flag = (volatile uint8_t *)(0x00010000 + 16);
	volatile uint8_t *miso_flag = (volatile uint8_t *)(0x00010000 + 24);
	volatile uint8_t *set_cs = (volatile uint8_t *)(0x00010000 + 32);
	volatile uint8_t *set_lsb_tranfer = (volatile uint8_t *)(0x00010000 + 40);
	volatile uint8_t *set_cpol = (volatile uint8_t *)(0x00010000 + 48);
	volatile uint8_t *set_cpha = (volatile uint8_t *)(0x00010000 + 56);
	// Assign Dummy value to set_cpol and set_cpha to determine when the user has written the CPOL and CPHA values
	*set_cpol = 4;
	*set_cpha = 4
	    //Set miso_flag to zero
	    * miso_flag = 0;
	//Set mosi_flag to zero
	*mosi_flag = 0;
	//set default value for lsb_first transfer
	*set_lsb_tranfer = 1;
	volatile uint8_t mosi;
	volatile uint8_t miso = 0x0;
	//Set the CFG Register to direct output instead of serial output
	CT_CFG.GPCFG0 = 0;
	//clear the r30 register
	__R30 = 0x0000;
	//clear the r31 register
	__R31 = 0x0000;
	while (1) {
		//Block until cpol_cpha_option is written
		while ((*set_cpol) + (*set_cpha) > 2) ;	//cpol and cpha option are either 0 or 1. 
		//set clock_pol_pha between 0 and 3
		uint8_t clock_pol_pha = (*set_cpol) * 2 + (*set_cpha);
		if (clock_pol_pha >= 2) {
			//set the clock 1 to be in its idle state
			__R30 |= (1 << CLK);
		}
		//      Block until Driver has written the mosi value
		while ((*mosi_flag == 0)) ;
		mosi = *mosi_transfer;
		if (*lsb_first) {
			switch (clock_pol_pha) {
			case 0:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}

				for (i = 0; i < 8; i++) {
					//Right shift LSB in miso
					miso = miso >> i;
					//Write Data on MOSI pin with LSB being first transferred
					if ((mosi << (7 - i)) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {	//This generates 0 at the specified bit and 1 at all others
						__R30 &= ~(1 << MOSI);
					}

					//set clk 1
					__R30 |= (1 << CLK);

					//set clk 0
					__R30 &= ~(1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x80;

					} else {

						miso & = ~(1 << 7);
					}

				}
				break;
			case 1:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					//Right shift LSB in miso
					miso = miso >> i;
					//Write Data on MOSI pin with LSB being first transferred
					if ((mosi << (7 - i)) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}

					//set clk 1
					__R30 |= (1 << CLK);
					//set clk 0
					__R30 &= ~(1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x80;

					} else {

						miso & = ~(1 << 7);
					}
				}
				break;

			case 2:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					//Right shift LSB in miso
					miso = miso >> i;
					//Write Data on MOSI pin with LSB being first transferred
					if ((mosi << (7 - i)) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}

					//set clk 0 to set it in active state
					__R30 &= ~(1 << CLK);
					//set clk 1 to idle state
					__R30 |= (1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x80;

					} else {

						miso & = ~(1 << 7);
					}
				}
				break;
			case 3:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					//Right shift LSB in miso
					miso = miso >> i;
					//Write Data on MOSI pin with LSB being first transferred
					if ((mosi << (7 - i)) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}
					//set clk 0 to set it in active state
					__R30 &= ~(1 << CLK);
					//set clk 1 to idle state
					__R30 |= (1 << CLK);
					if (__R31 & 0x20) {
						miso |= 0x80;

					} else {

						miso & = ~(1 << 7);
					}

				}

				break;
			default:
				break;

			}

		} else {
			switch (clock_pol_pha) {
			case 0:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}

				for (i = 0; i < 8; i++) {
					//Left shift LSB in miso
					miso = miso << i;
					//Write Data on MOSI pin with MSB being first transferred
					if ((mosi << i) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {	//This generates 0 at the specified bit and 1 at all others
						__R30 &= ~(1 << MOSI);
					}

					//set clk 1
					__R30 |= (1 << CLK);

					//set clk 0
					__R30 &= ~(1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x1;

					} else {

						miso & = ~(1 >> 7);
					}

				}
				break;
			case 1:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					//Left shift LSB in miso
					miso = miso << i;
					//Write Data on MOSI pin with MSB being first transferred
					if ((mosi << i) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}

					//set clk 1
					__R30 |= (1 << CLK);
					//set clk 0
					__R30 &= ~(1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x1;

					} else {

						miso & = ~(1 >> 7);
					}
				}
				break;

			case 2:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					// Left shift LSB in miso
					miso = miso << i;
					//Write Data on MOSI pin with MSB being first transferred
					if ((mosi << i) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}

					//set clk 0 to set it in active state
					__R30 &= ~(1 << CLK);
					//set clk 1 to idle state
					__R30 |= (1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x1;

					} else {

						miso & = ~(1 >> 7);
					}
				}
				break;
			case 3:
				// set CS low or High according to set_cs
				if (!(*set_cs)) {
					__R30 &= ~(1 << CS);
				}

				else {
					__R30 |= (1 << CS);
				}
				for (i = 0; i < 8; i++) {
					//Left shift MSB in miso
					miso = miso << i;
					//Write Data on MOSI pin with MSB being first transferred
					if ((mosi << i) & 0x80) {
						__R30 |= (1 << MOSI);
					} else {
						__R30 &= ~(1 << MOSI);
					}
					//set clk 0 to set it in active state
					__R30 &= ~(1 << CLK);
					//set clk 1 to idle state
					__R30 |= (1 << CLK);

					if (__R31 & 0x20) {
						miso |= 0x1;

					} else {

						miso & = ~(1 >> 7);
					}

				}

				break;
			default:
				break;

			}

		}

		*miso_transfer = 1;
		*mosi_transfer = 0;
	}

	__halt();
}
