/*
Memory (memory.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Contains convenience functions which wrap the PMM, VMM, and paging into a simpler interface.
*/
#include <stddef.h>
#include <stdint.h>
#include "memory.h"
#include "kernel.h"
#include "paging.h"
#include "pmm.h"
#include "vmm.h"

uint32_t* get_address_space(size_t size_in_bytes)
{
	Output("\nAddress space of %d bytes requested.", size_in_bytes);
	size_t size_in_pages = size_in_bytes/0x1000;
	Output("\nThis will consist of %d pages.", size_in_pages);
	uint32_t num_tables = size_in_pages/1024;
	if (size_in_pages%1024 > 0) num_tables++; //Always need one table!
	Output("\nIt will require %d page tables.", num_tables);
	uint32_t* PGD = pmm_alloc();
	Output("\nThe page directory will be in a page at 0x%x.", (uint32_t)PGD);
	uint32_t address = 0;
	for (int i = 0; i < num_tables; i++)
	{
		uint32_t* PGT = PGD + 1024 + (entry_size * i);
		uint32_t entry_max = 1024;
		//If this is the last table, use the remainder if applicable.
		if (i + 1 == num_tables && (size_in_pages%1024 > 0)) entry_max = size_in_pages%1024;	
		Output("\nNew page table (#%d) at 0x%x with %d entries.", i, (uint32_t)PGT, entry_max);		
		for (uint32_t j = 0; j < entry_max; j++)
		{
			uint32_t* page = pmm_alloc();
			PGT[j] = (uint32_t)page | 0x3; //Kernel Page
			Output("\nMapped 0x%x to physical page at 0x%x (entry #%d).", address, (uint32_t)page, j);
			address += page_size;
		}
		PGD[i] = (uint32_t)PGT; 
		PGD[i] |= 0x3; //Kernel Table
	}
	return PGD;
}