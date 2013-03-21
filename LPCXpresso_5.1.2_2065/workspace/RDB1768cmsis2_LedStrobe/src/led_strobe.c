//*****************************************************************************
//  The tinniest C blink program for LPC17xx - 28 bytes + vec. checksum = 32 B
//  Copyright (c) 2011 gbm
//*****************************************************************************
#include "LPC17xx.h"

#define LED_PORT	LPC_GPIO0
#define LED_MASK (1 << 22)

// set shift (left or right) so that LED state is controlled by bit 19..21
// of free-running counter variable
#define SHIFT(a)	(a << 3)

void ResetISR(void);
//*****************************************************************************
// External declaration for the pointer to the stack top from the Linker Script
//*****************************************************************************
extern void _vStackTop(void);

//*****************************************************************************
// The vector table.
// This relies on the linker script to place at correct location in memory.
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
	&_vStackTop,	// The initial stack pointer
	ResetISR		// The reset handler
	//0, 0, 0, 0, 0, 0
};

//*****************************************************************************
// Reset entry point
//*****************************************************************************
__attribute__ ((section(".code_at_8")))
void ResetISR(void)
{
	uint32_t i;		// real men don't need to initialize variables

	for (;; i ++)
	{
		LED_PORT->FIODIR = LED_MASK;	// makes the code longer when placed before the loop
		LED_PORT->FIOPIN = SHIFT(i);
	}
}

__attribute__ ((section(".data_at_1c")))
uint32_t checksum_placeholder = 0;
