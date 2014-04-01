/*
Physical Memory Manager (pmm.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Manages and provides allocation/deallocation functionality for physical memory.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pmm.h"
#include "kernel.h"

void pmm_free(void* page)
{
	uint32_t* entry = (uint32_t*)page;
	*entry = (uint32_t)current_page;
	current_page = entry;
	free_pages++;
}

uint32_t pmm_alloc()
{
	uint32_t ret = (uint32_t)current_page;
	current_page = (uint32_t*)*current_page;
	free_pages--;
	return ret;
}

void pmm_claim(uint32_t* address, size_t size)
{
	int num_pages = size/4096;
	Output("%d Pages Present.", num_pages);
	for (int i = 0; i < num_pages; i++)
	{
		uint32_t* new_page = address + (i*4096);
		if (new_page > (uint32_t*)0xFFFFF && new_page < (uint32_t*)0x500000)
			Output("\nKernel Page At 0x%x, Skipping..", new_page);
		else 
		{
			Output("\nPage Claimed At 0x%x (%d).", new_page, free_pages);
			*current_page = (uint32_t)new_page;
			current_page = new_page;
			free_pages++;
		}
	}
}