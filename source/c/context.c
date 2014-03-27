/*
Context-Switching (context.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the code to switch contexts, and store/load contexts from memory.
*/

#include <stdint.h>

extern void ctxt_sw(void** oldStack, void* newStack);

uint8_t get_ring()
{
	return 0; //placeholder
}

void set_ring(uint8_t level)
{
	return; //placeholder
}

void context_switch(void** oldStack, void* newStack)
{
	ctxt_sw(oldStack, newStack);
}