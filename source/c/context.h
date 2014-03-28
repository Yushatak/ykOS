/*
Context-Switching Header (context.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the definitions of structures and functions
to switch contexts, and store/load contexts from memory.
*/

#include <stdint.h>

void context_switch(void** oldStack, void* newStack);