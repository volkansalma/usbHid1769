//*****************************************************************************
//   +--+       
//   | ++----+   
//   +-++    |  
//     |     |  
//   +-+--+  |   
//   | +--+--+  
//   +----+    Copyright (c) 2009-11 Code Red Technologies Ltd.
//
// main_ledflash.c demonstrates the use of the "user LEDs" on the
// RDB1768 development board (LEDs 2-5).
//
//
// Software License Agreement
// 
// The software is owned by Code Red Technologies and/or its suppliers, and is 
// protected under applicable copyright laws.  All rights are reserved.  Any 
// use in violation of the foregoing restrictions may subject the user to criminal 
// sanctions under applicable laws, as well as to civil liability for the breach 
// of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
// TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
// CODE RED TECHNOLOGIES LTD. 

#include "leds.h"

#include "LPC17xx.h"
#include <cr_section_macros.h>
#include <NXP/crp.h>

#define LED_PORT	LPC_GPIO0
#define LED_MASK (1 << 22)

// set shift (left or right) so that LED state is controlled by bit 19..21
// of free-running counter variable
#define SHIFT(a)	(a << 3)


// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;

// Function to provide short delay
void short_delay (int n) __attribute__((noinline));
void short_delay(int n)
{
   volatile int d;
   for (d=0; d<n*3000; d++){}
}

int main(void)
{
	// Initialise GPIO to access user LEDs
	leds_init();
	
	// Enter an infinite loop, cycling through led flash sequence
	while(1)
	{

			leds_all_on();
			short_delay(1000);
			leds_all_off();		
			short_delay(1000);
	}
	
	return 0 ;
}
