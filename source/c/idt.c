/*
Interrupt Descriptor Table (idt.c)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

This contains code to generate the entire IDT on-the-fly, and then load it into the
system. It links to various things in "isr.asm".
*/
#include <stdint.h>
#include "kernel.h"
#include "inline.h"
#include "screen.h"

//External Symbols
//Interrupt Service Routine Wrappers (from ISR.ASM)
extern isr_registers_t* ISR0, ISR1, ISR2, ISR3, ISR4, ISR5, ISR6, ISR7, ISR8, ISR9, ISR10, ISR11, ISR12, ISR13, ISR14, ISR15, ISR16, ISR17, ISR18, ISR19, ISR20, ISR21, ISR22, ISR23, ISR24, ISR25, ISR26, ISR27, ISR28, ISR29, ISR30, ISR31;
//Software Interrupts (from ISR.ASM)
extern isr_registers_t* ISR48;
//Interrupt Request Wrappers (from ISR.ASM)
extern isr_registers_t* IRQ0, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7, IRQ8, IRQ9, IRQ10, IRQ11, IRQ12, IRQ13, IRQ14, IRQ15;
uint32_t ISR[256];
idt_entry_t idt_entries[256];

//Static Declarations
static void setIDT(uint8_t idx, uint32_t base, uint16_t selector, uint8_t flags);

void generateIDT()
{ 
	ISR[0] = (uint32_t)&ISR0;
	ISR[1] = (uint32_t)&ISR1;
	ISR[2] = (uint32_t)&ISR2; //cascade
	ISR[3] = (uint32_t)&ISR3;
	ISR[4] = (uint32_t)&ISR4;
	ISR[5] = (uint32_t)&ISR5;
	ISR[6] = (uint32_t)&ISR6;
	ISR[7] = (uint32_t)&ISR7;
	ISR[8] = (uint32_t)&ISR8;
	ISR[9] = (uint32_t)&ISR9;
	ISR[10] = (uint32_t)&ISR10;
	ISR[11] = (uint32_t)&ISR11;
	ISR[12] = (uint32_t)&ISR12;
	ISR[13] = (uint32_t)&ISR13;
	ISR[14] = (uint32_t)&ISR14;
	ISR[15] = (uint32_t)&ISR15;
	ISR[16] = (uint32_t)&ISR16;
	ISR[17] = (uint32_t)&ISR17;
	ISR[18] = (uint32_t)&ISR18;
	ISR[19] = (uint32_t)&ISR19;
	ISR[20] = (uint32_t)&ISR20;
	ISR[21] = (uint32_t)&ISR21;
	ISR[22] = (uint32_t)&ISR22;
	ISR[23] = (uint32_t)&ISR23;
	ISR[24] = (uint32_t)&ISR24;
	ISR[25] = (uint32_t)&ISR25;
	ISR[26] = (uint32_t)&ISR26;
	ISR[27] = (uint32_t)&ISR27;
	ISR[28] = (uint32_t)&ISR28;
	ISR[29] = (uint32_t)&ISR29;
	ISR[30] = (uint32_t)&ISR30;
	ISR[31] = (uint32_t)&ISR31;
	ISR[32] = (uint32_t)&IRQ0;
	ISR[33] = (uint32_t)&IRQ1;
	ISR[34] = (uint32_t)&IRQ2;
	ISR[35] = (uint32_t)&IRQ3;
	ISR[36] = (uint32_t)&IRQ4;
	ISR[37] = (uint32_t)&IRQ5;
	ISR[38] = (uint32_t)&IRQ6;
	ISR[39] = (uint32_t)&IRQ7;
	ISR[40] = (uint32_t)&IRQ8;
	ISR[41] = (uint32_t)&IRQ9;
	ISR[42] = (uint32_t)&IRQ10;
	ISR[43] = (uint32_t)&IRQ11;
	ISR[44] = (uint32_t)&IRQ12;
	ISR[45] = (uint32_t)&IRQ13;
	ISR[46] = (uint32_t)&IRQ14;
	ISR[47] = (uint32_t)&IRQ15;
	ISR[48] = (uint32_t)&ISR48;
	
	memFill(idt_entries, sizeof(idt_entry_t)*256, 0);
	for (int i = 0; i < 256; i++)
	{
		if (i != 2) setIDT(i, (uint32_t)ISR[i], 0x08, 0x8E);
	}
	lidt((int)&idt_entries, sizeof(idt_entry_t)*256);
}

static void setIDT(uint8_t idx, uint32_t base, uint16_t selector, uint8_t flags)
{
	idt_entries[idx].base_lo = base & 0xFFFF;
	idt_entries[idx].base_hi = (base >> 16) & 0xFFFF;
	idt_entries[idx].selector = selector;
	idt_entries[idx].unused = 0;
	idt_entries[idx].flags = flags; // | 0x60 to give privilege level 3 user mode stuff for later.
}

inline void lidt(uint32_t base, uint16_t size)
{
	struct
	{
		uint16_t length;
		uint32_t base;
	} __attribute__((packed)) IDTR;
	
	IDTR.length = size;
	IDTR.base = base;
	__asm__ volatile ("lidt [%0]" : : "m"(IDTR));
	__asm__ volatile ("sti");
}

void Interrupt(isr_registers_t* regs)
{	
	if (regs->intvec >= 0x28) outb(0xA0, 0x20); //Reset Slave PIC
	outb(0x20, 0x20); //Reset Master PIC
	
	if (intHandlers[regs->intvec] != 0)
	{
		((isr_t)intHandlers[regs->intvec])(regs);
	}
	else if (regs->intvec < 0x20 || regs->intvec > 0x30)
	{
		Output("\nUnhandled Interrupt %d (0x%x)!", regs->intvec, regs->intvec);
		Output("\nFault At EIP: 0x%x", regs->eip);
		panic();
		__asm__ volatile("hlt");
	}
}

void registerISR(uint8_t idx, isr_t isr)
{
	intHandlers[idx] = isr;
}