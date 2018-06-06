/*
Physical Memory Manager (pmm.c)
Part of the ykOS Project
Written by Johnathan "Yushatak" Schur
Copyright Yushatak 2014-2018
All Rights Reserved

Manages and provides allocation/deallocation functionality for physical memory.

Works by maintaining a linked list of free pages, adding or removing pages as we claim/free or hand out pages, and 
maintaining a count of free pages so that we don't go past the end of the linked list or try to hand out pages we don't have.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "pmm.h"
#include "kernel.h"
#include "inline.h"

uint32_t free_pages = 0; //Prevents traversing the linked list of free pages too far by keeping track of the number of pages it has.

//Frees a page by putting it at the end of the linked list and making it the current page (so it would be the next to be handed out).
void pmm_free(void* page)
{
	uint32_t* entry = (uint32_t*)page; //Cast the input pointer to uint32_t* to access as unsigned 32-bit values.
	*entry = (uint32_t)current_page; //Set the first 32-bit value at the pointer to the current page being tracked by the PMM.
	current_page = entry; //Change the current page being tracked to the newly-freed page.
	free_pages++; //Increment free page count.
}

//Allocates a page by changing the current page to the next one up the list, decrementing the number of free pages and finally
//handing out what was (until now) the current page.
uint32_t* pmm_alloc()
{
	uint32_t* ret = current_page; //Set the return value to the current page, which we are going to give out.
	current_page = (uint32_t*)*current_page; //Update the current page up the linked list to the next entry.
	free_pages--; //Decrement the number of free pages.
	return ret; //Return the page.
}

//Take a chunk of provided memory at "address" of "size" and split it into pages, linking them together and incrementing the free page counter
//and current page as we go.
void pmm_claim(uint32_t* address, size_t size)
{
	int num_pages = size/0x1000; //Figure out how many pages the provided size equates to.
	for (int i = 0; i < num_pages; i++) //Loop through the number of pages we determined..
	{
		uint32_t* new_page = address + (i * (0x1000/sizeof(uint32_t))); //Calculate the address of the next page and make it a pointer.
		if ((uint32_t)new_page > 0x44FFFF && (uint32_t)new_page < (GetMemoryCount()*1024) && new_page != NULL) //If the new page is above 44FFFF (don't want to murder the kernel or its friends!), not past the end of memory, and the pointer isn't to nothing..
		{
			*new_page = (uint32_t)current_page; //Make the new page point to what was the currently tracked page.
			current_page = new_page; //Make the new page the currently tracked page.
			free_pages++; //Increment the number of free pages.
		}
	}
}