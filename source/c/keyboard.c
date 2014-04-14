/*
Keyboard.c (keyboard.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This provides translation functions for keycodes, scancodes, chars, and so on.
*/
#include <stdint.h>
#include "keyboard.h"
#include "inline.h"

uint32_t charsToInt(char* in)
{
	uint32_t total = 0;
	while (*in)
	{
		total = (total * 16) + char_nybbles[*(uint8_t * )in++];
	}
	return total;
}

void initialize_keyboard()
{
	keybuffer[0] = 0;
	keybuffer_ptr = 0;
	cmdbuffer[0] = 0;
	alt = false;
	ctrl = false;
	shift = false;
}