/*UNUSED*/
/*
Virtual Memory Manager (vmm.c)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

Manages and provides allocation/deallocation functionality for virtual memory.
*/

/*
all physical memory is statically mapped, and then a virtual memory manager 
has control of the rest of the address space, primarily for stacks but also 
mmio and swapping and etc. - if a machine has 4GB+ physical, then reserve 10% 
if PAE isn't supported, otherwise enable PAE. if a machine has more than 64GB 
then reserve 10% if 64-bit isn't supported, otherwise use long mode
*/

#include "vmm.h"
#include <stdint.h>
#include <stdbool.h>

const int VME_COUNT = 32768;
vm_table_t* VMT = (void*)0x500000;

uint32_t vmm_alloc(uint16_t tid)
{
	for (unsigned int i = 0; i < VME_COUNT; i++)
	{
		vm_entry_t* VME = &(VMT->entries[i]);
		if (VME->owner == 0)
		{
			VME->owner = tid;
			return (i*0x1000)+0xFF000000;
		}
	}
	return 0; //Failure to retrieve page. none free!
}

void vmm_free(uint32_t vaddr)
{
	vm_entry_t* VME = &(VMT->entries[(vaddr-0xFF000000)/0x1000]);
	VME->owner = 0;
}

bool vmm_check(uint32_t vaddr)
{
	vm_entry_t* VME = &(VMT->entries[(vaddr-0xFF000000)/0x1000]);
	return (VME->owner == 0);
}