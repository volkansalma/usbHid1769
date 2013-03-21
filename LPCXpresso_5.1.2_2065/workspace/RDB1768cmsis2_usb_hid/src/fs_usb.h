/*
 * fs_usb.h
 *
 *  Created on: 21 Mar 2013
 *      Author: Volkan
 */

#include <stdio.h>

#include <cr_section_macros.h>

#ifndef FS_USB_H_
#define FS_USB_H_

#define INTR_IN_EP		0x81

#define MAX_PACKET_SIZE	64

#define LE_WORD(x)		((x)&0xFF),((x)>>8)

#define REPORT_SIZE			8



void fs_USB_Init();

void fs_USB_interrupt_handler();

void fs_USB_delay_for_send_part(int n) __attribute__((noinline));

void fs_USB_delay_for_send_part(int n);

void fs_USB_send_usb_data(unsigned char *pArray, int dataSize);

#endif /* FS_USB_H_ */
