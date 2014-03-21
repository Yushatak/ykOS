/*
Virtual Memory Manager Header (vmm.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains declarations for the functions to manage and provide allocation/deallocation functionality for virtual memory.
*/

typedef struct vm_entry
{
	uint8_t t_index, p_index;
	uint16_t owner;
} vm_entry_t;

typedef struct vm_table
{
	vm_entry_t entries[32768];
} vm_table_t;

//Functions
//uint32_t malloc(uint32_t pageCount);
uint32_t palloc(uint16_t tid);
void free(uint32_t vaddr);
bool check(uint32_t vaddr);