/*
 * sw.c
 * e2417105
 */
#include <stdint.h>
#include "iodefine.h"
#include "sw.h"

void sw_init(void)
{
	PORT5.PDR.BYTE &= 0xF0;
	PORTH.PDR.BYTE &= 0xF0;
}

uint8_t sw_read(uint8_t sw)
{
	if (1 <= sw && sw <= 4)
	{
		return (PORT5.PIDR.BYTE >> (sw - 1)) & 0x01;
	}
	else if (5 <= sw && sw <= 8)
	{
		return (PORTH.PIDR.BYTE >> (sw - 5)) & 0x01;
	}
	else
	{
		return SW_OFF;
	}
}

