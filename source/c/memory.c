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

uint32_t GetEntry(int address)
{
	return (uint32_t)(((page_table_t*)(PGD[address/4096] & (~0x7)))->entries[address/1024%4]);
}

void SetEntry(int address, uint32_t value)
{
	((page_table_t*)(PGD[address/4096] & (~0x7)))->entries[address/1024%4] = value;
}

uint32_t GetPageEntry(void* table, int index)
{
	return ((page_table_t*)table)->entries[index];
}

void PutPageEntry(void* table, int index, uint32_t entry)
{
	((page_table_t*)table)->entries[index] = entry;
}

void PutPageEntryA(int address, uint32_t entry)
{
	((page_table_t*)(((page_table_t*)(get_cr3() & (~0xFFF)))->entries[address/1024/4]))->entries[address/1024%4] = entry;
}

uint32_t GetPageA(int address)
{
	return ((page_table_t*)(get_cr3() & (~0xFFF)))->entries[address/4096] & (~0x7);
}

void EnableTable(int address)
{
	page_table_t* PGT = ((page_table_t*)(get_cr3() & (~0xFFF)));
	PGT->entries[address/4096] &= (~0x7);
	PGT->entries[address/4096] |= 0x3;
}

uint32_t GetEntryA(int address)
{
	return (uint32_t)(((page_table_t*)GetPageA(address))->entries[address/1024%4] & (~0x7));
}

uint32_t GetPageEntryA(int address)
{
	return (uint32_t)(((page_table_t*)(((page_table_t*)(get_cr3() & (~0xFFF)))->entries[address/4096] & (~0x7)))->entries[address/1024%4] & (~0x7));
}