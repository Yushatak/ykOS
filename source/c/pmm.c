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

uint32_t free_pages = 0;

void pmm_free(void* page)
{
	uint32_t* entry = (uint32_t*)page;
	*entry = (uint32_t)current_page;
	current_page = entry;
	free_pages++;
}

uint32_t* pmm_alloc()
{
	uint32_t* ret = current_page;
	current_page = (uint32_t*)*current_page;
	free_pages--;
	return ret;
}

void pmm_claim(uint32_t* address, size_t size)
{
	int num_pages = size/0x1000;
	for (int i = 0; i < num_pages; i++)
	{
		uint32_t* new_page = address + (i*0x1000);
		if ((uint32_t)new_page > 0x6FFFFF && (uint32_t)new_page < (GetMemoryCount()*1024))
		{
			*current_page = (uint32_t)new_page;
			current_page = new_page;
			free_pages++;
		}
	}
}