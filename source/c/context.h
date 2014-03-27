/*
Context-Switching Header (context.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the definitions of structures and functions
to switch contexts, and store/load contexts from memory.
*/

uint8_t get_ring()
void set_ring(uint8_t level);
void context_switch(void** oldStack, void* newStack);