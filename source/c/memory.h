/*
Memory Header (memory.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

This contains declarations for the functions to set up and manipulate memory access.
*/

#include <stdint.h>
#include <stddef.h>

//External Functions
extern void A20();
extern void LinearGDT();

//Function Declarations
uint32_t* get_address_space(size_t size_in_bytes);
uintptr_t get_linked_pages(size_t size_in_bytes);
void free_linked_pages(uintptr_t page_address);

//Inline