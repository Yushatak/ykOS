/*
Memory (memory.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Contains functions related to setting up and managing memory.
*/

#include <stdint.h>
#include "memory.h"
#include "kernel.h"
extern void* kernel_end;
uint32_t* PGD = (uint32_t*)&kernel_end;

void EnablePaging()
{
	//Generate Page Directory, Tables, & Entries for 4GB Identity Mapped
	uint32_t address = 0;
	for (int i = 0; i < 1024; i++)
	{
		uint32_t* PGT = PGD + 1024 + (32768 * i);
		for (uint32_t i = 0; i < 1023; i++)
		{
			PGT[i] = address | 3; //Kernel Page
			address += 0x1000;
		}
		PGD[i] = (uint32_t)PGT; 
		PGD[i] |= 0x3; //Kernel Table
	}
	__asm__ volatile ("mov cr3, %0" :: "b"(PGD));
	__asm__ volatile ("mov cr0, %0" :: "b"(get_cr0() | 0x80000000));
}

uint32_t Get_ID_PTE(int address)
{
	uint32_t table_index = address/4096;
	uint32_t page_index = address/1024%4;
	page_table_t* PGT = (void*)(PGD[table_index] & 0xFFFFF000);
	uint32_t ret = PGT->entries[page_index];
	return ret;
	//return (uint32_t)(((page_table_t*)(PGD[address/4096] & (~0xFFF)))->entries[address/1024%4]);
}

void Set_ID_PTE(int address, uint32_t value)
{
	uint32_t table_index = address/4096;
	uint32_t page_index = address/1024%4;
	page_table_t* PGT = (void*)(PGD[table_index] & 0xFFFFF000);
	PGT->entries[page_index] = value;
}