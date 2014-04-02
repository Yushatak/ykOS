/*
Paging (paging.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Contains functions related to setting up and managing page tables.
*/

#include <stdint.h>
#include "kernel.h"
#include "paging.h"
extern void* kernel_end;

void EnablePaging()
{
	//Generate Page Directory, Tables, & Entries for 4GB Identity Mapped
	uint32_t* PGD = (uint32_t*)&kernel_end;
	uint32_t address = 0;
	for (int i = 0; i < 1024; i++)
	{
		uint32_t* PGT = PGD + 1024 + (entry_size * i);
		for (uint32_t j = 0; j < 1024; j++)
		{
			PGT[j] = address | 0x3; //Kernel Page
			address += page_size;
		}
		PGD[i] = (uint32_t)PGT; 
		PGD[i] |= 0x3; //Kernel Table
	}
	__asm__ volatile ("mov cr3, %0" :: "b"(PGD));
	__asm__ volatile ("mov cr0, %0" :: "b"(get_cr0() | 0x80000000));
}

inline uint32_t table_index(uint32_t vaddr)
{
	return (vaddr/4096)/1024;
}

inline uint32_t page_index(uint32_t vaddr)
{
	return (vaddr/4096)%1024;
}

void Disable_PTE(uint32_t vaddr)
{
	Set_Flags_PTE(vaddr, 0x7FFFFFFF);
}

void Enable_PTE(uint32_t vaddr)
{
	Set_Flags_PTE(vaddr, 0x80000000);
}

void Set_Flags_PTE(uint32_t vaddr, uint32_t flags)
{
	uint32_t* PGD = (uint32_t*)&kernel_end;
	page_table_t* PGT = (void*)(PGD[table_index(vaddr)] & 0xFFFFF000);
	uint32_t old = PGT->entries[page_index(vaddr)];
	PGT->entries[page_index(vaddr)] = old & flags;
	reload_cr3();
}

void Map_PTE(uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
	Set_PTE(vaddr, paddr & flags);
}

uint32_t Get_PTE(uint32_t vaddr)
{
	uint32_t* PGD = (uint32_t*)&kernel_end;
	page_table_t* PGT = (void*)(PGD[table_index(vaddr)] & 0xFFFFF000);
	uint32_t ret = PGT->entries[page_index(vaddr)];
	return ret;
}

void Set_PTE(uint32_t vaddr, uint32_t value)
{
	uint32_t* PGD = (uint32_t*)&kernel_end;
	page_table_t* PGT = (void*)(PGD[table_index(vaddr)] & 0xFFFFF000);
	PGT->entries[page_index(vaddr)] = value;	
	reload_cr3();
}

void Mirror_PTE(uint32_t vaddr, uint32_t vaddr2)
{
	Set_PTE(vaddr, Get_PTE(vaddr2));
}

inline void invlpg(uint32_t vaddr)
{
	__asm__ volatile ("invlpg [%0]" :: "a"(vaddr));
}

inline void reload_cr3()
{
	__asm__ volatile ("mov cr3, %0" :: "b"(get_cr3()));
}