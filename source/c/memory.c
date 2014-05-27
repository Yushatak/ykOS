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
#include "screen.h"

uint32_t* get_address_space(size_t size_in_bytes)
{
	size_t size_in_pages = size_in_bytes/0x1000;
	uint32_t num_tables = size_in_pages/1024;
	if (size_in_pages%1024 > 0) num_tables++; //Always need one table!
	uint32_t* PGD = pmm_alloc();
	uint32_t address = 0;
	for (int i = 0; i < num_tables; i++)
	{
		uint32_t* PGT = PGD + 1024 + (entry_size * i);
		uint32_t entry_max = 1024;
		//If this is the last table, use the remainder if applicable.
		if (i + 1 == num_tables && (size_in_pages%1024 > 0)) entry_max = size_in_pages%1024;		
		uint32_t j;
		for (j = 0; j < entry_max; j++)
		{
			uint32_t* page = pmm_alloc();
			PGT[j] = (uint32_t)page | 0x7; //User Page
			address += page_size;
		}
		if (i + 1 == num_tables)
			PGT[j+1] = (uint32_t)PGD | 0x5; //Read-only User Page
		PGD[i] = (uint32_t)PGT; 
		PGD[i] |= 0x7; //User Table
	}
	return PGD;
}

uintptr_t get_linked_pages(size_t size_in_bytes)
{
	size_t page_count = size_in_bytes/4096;
	if (size_in_bytes%4096 > 0) page_count++; //Account for partial page.
	uint32_t pages[page_count]; //Temporary list while building linked list.
	uintptr_t ret = 0; //Return value - will be address of first page.
	for (int i = 0; i < page_count; i++)
	{
		//Allocate page and put into linked list.
		uint32_t paddr = (uint32_t)pmm_alloc();
		pages[i] = paddr;
		uint32_t* page = (uint32_t*)pages[i];
		page[0] = 0; //Explicitly clear entry from old linked list this page may have been in.
		if (ret == 0) ret = pages[i]; //Assign first page as ret.
		else ((uint32_t*)pages[i-1])[0] = (uint32_t)page;
	}
	uint32_t* last_page = (uint32_t*)pages[page_count - 1];
	last_page[0] = pages[0]; //Loop the list.
	return ret;
}