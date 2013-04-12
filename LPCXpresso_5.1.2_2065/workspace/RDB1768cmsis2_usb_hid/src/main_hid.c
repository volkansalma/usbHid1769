/*
	LPCUSB, an USB device driver for LPC microcontrollers
	Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright
	   notice, this list of conditions and the following disclaimer in the
	   documentation and/or other materials provided with the distribution.
	3. The name of the author may not be used to endorse or promote products
	   derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
	IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
	OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
	IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
	INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
	THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//CodeRed
//Added ref to stdio.h to pull in semihosted printf rather than using serial
#include <stdio.h>

#include <cr_section_macros.h>
#include <NXP/crp.h>

#include "fs_usb.h"
#include "lpc17xx_can.h"
#include "lpc17xx_pinsel.h"

// Variable to store CRP value in. Will be placed automatically
// by the linker when "Enable Code Read Protect" selected.
// See crp.h header for more information
__CRP const unsigned int CRP_WORD = CRP_NO_CRP ;






/*************************************************************************
	main
	====
**************************************************************************/
CAN_MSG_Type TXMsg, RXMsg; // messages for test Bypass mode
CAN_MSG_Type TX2Msg, RX2Msg;
uint32_t CANRxCount, CANTxCount = 0;
uint32_t CAN2RxCount, CAN2TxCount = 0;




unsigned char arrayTest[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18}; //6 nın katı olmalı

int newDataToSend = 0;

void CAN_IRQHandler()
{
	uint8_t CAN1IntStatus;
	uint8_t CAN2IntStatus;
	CAN1IntStatus = CAN_IntGetStatus(LPC_CAN1);
	CAN2IntStatus = CAN_IntGetStatus(LPC_CAN2);
	//check receive interrupt
	if(CAN1IntStatus &0x01)
	{
		CAN_ReceiveMsg(LPC_CAN1,&RXMsg);
		CANRxCount++; //count success received message
		TXMsg.id  = RXMsg.id + 0x100;

		CAN_SendMsg(LPC_CAN1, &TXMsg);
		CANTxCount++;
	}
	if(CAN2IntStatus &0x01)
	{
		CAN_ReceiveMsg(LPC_CAN2,&RX2Msg);
		CAN2RxCount++; //count success received message
		TX2Msg.id  = RX2Msg.id + 0x100;
		CAN_SendMsg(LPC_CAN2, &TXMsg);
		CAN2TxCount++;
	}

}


void CAN_InitMessage(void)
{
	TXMsg.format = STD_ID_FORMAT;
	TXMsg.id = 0x123;
	TXMsg.len = 8;
	TXMsg.type = DATA_FRAME;


	TX2Msg.format = STD_ID_FORMAT;
	TX2Msg.id = 0x126;
	TX2Msg.len = 8;
	TX2Msg.type = DATA_FRAME;


	RXMsg.format = STD_ID_FORMAT;
	RXMsg.id = 0x00;
	RXMsg.len = 0x00;
	RXMsg.type = 0x00;
	RXMsg.dataA[0] = RXMsg.dataA[1] = RXMsg.dataA[2] = RXMsg.dataA[3] = 0x00000000;
	RXMsg.dataB[0] = RXMsg.dataB[1] = RXMsg.dataB[2] = RXMsg.dataB[3] = 0x00000000;
}


PINSEL_CFG_Type PinCfg;
int main(void)
{
	int count = 0;
	fs_USB_Init();

	PINSEL_CFG_Type PinCfg;
	/* Pin configuration
	* CAN1: select 0.0 as RD1. P0.1 as TD1
	*/
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);

	// CAN2 Pins P0.4 as RD2 and P0.85 as TD2
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 4;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);


	//Initialize CAN1
	CAN_Init(LPC_CAN1, 125000);
	CAN_Init(LPC_CAN2, 125000);

	//Enable Interrupt
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);
	CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);
	CAN_SetAFMode(LPC_CANAF,CAN_AccBP);
	CAN_InitMessage();
	CAN_SendMsg(LPC_CAN1, &TXMsg);
	CAN_SendMsg(LPC_CAN2, &TXMsg);

	CAN_ModeConfig(LPC_CAN1, CAN_SELFTEST_MODE, ENABLE);
	CAN_ModeConfig(LPC_CAN2, CAN_SELFTEST_MODE, ENABLE);

	// call USB interrupt handler continuously
	while (1)
	{

		/*
		if(CAN_ReceiveMsg(LPC_CAN1,&RXMsg) == SUCCESS)
		{
			newDataToSend = 1;
		}
		*/

		if(newDataToSend)
		{
			newDataToSend = 0;
			fs_USB_send_usb_data(&arrayTest[0], sizeof(arrayTest));

		}

		count++;
		if(count > 90000)
		{
			count = 0;
			newDataToSend = 1;
		}


		 CAN_SendMsg(LPC_CAN1, &TXMsg);
		 CAN_SendMsg(LPC_CAN2, &TXMsg);
	}

	return 0;
}
