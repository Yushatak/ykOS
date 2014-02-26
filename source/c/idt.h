/*
Interrupt Descriptor Table Header (idt.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains declarations for the functions to generate the entire IDT on-the-fly, as well
as load it into the system.
*/
#include <stdint.h>

void generateIDT();
void lidt(uint32_t base, uint16_t size);
