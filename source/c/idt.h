/*
Interrupt Descriptor Table Header (idt.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

This contains declarations for the functions to generate the entire IDT on-the-fly, as well
as load it into the system.
*/

typedef struct idt_entry_struct
{
	uint16_t base_lo;
	uint16_t selector;
	uint8_t unused;
	uint8_t flags;
	uint16_t base_hi;
} idt_entry_t;

typedef struct isr_registers
{
	uint32_t gs, fs, es, ds, ss, esp;
	uint32_t edi, esi, ebp, useless_esp, ebx, edx, ecx, eax;
	uint32_t intvec, ec;
	uint32_t eip, cs, eflags, intermittent_esp, intermittent_ss;
} isr_registers_t;

typedef void (*isr_t)(isr_registers_t*);
isr_t intHandlers[256];

//Function Definitions
void registerISR(uint8_t idx, isr_t ISR);
void generateIDT();
void lidt(uint32_t base, uint16_t size);