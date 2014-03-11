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

uint32_t GetEntryA(int address)
{
	return (uint32_t)(((page_table_t*)GetPageA(address))->entries[address/1024%4] & (~0x7));
}

uint32_t GetPageEntryA(int address)
{
	return (uint32_t)(((page_table_t*)(((page_table_t*)(get_cr3() & (~0xFFF)))->entries[address/4096] & (~0x7)))->entries[address/1024%4] & (~0x7));
}