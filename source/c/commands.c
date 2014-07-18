/*
Commands (commands.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the code for individual built-in command applets in the kernel shell.
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "kernel.h"
#include "keyboard.h"
#include "screen.h"
#include "inline.h"
#include "paging.h"
#include "commands.h"
#include "memory.h"
#include "vmm.h"
#include "drivers/ykfs.h"

#define BOCHS_BP() __asm__ volatile ("xchg bx,bx");

void cmd_Convert(char* args)
{
	char result[keybuffer_size*6]; //up to 6 chars out per input char, so multiply by 6
	streamToHex((unsigned char*)args, result, sizeof(result));
	Output(result);
}

void cmd_Peek(char* args, size_t argsize)
{
	Output(args);
	Output(":");
	char Chars[32] = {0};
	uintToHexChars(*(uint32_t *)charsToInt(args), Chars, 32);
	Output(Chars);
}

void cmd_Creg(void)
{
	char Chars[32] = {0};
	uintToHexChars(get_cr0(), Chars, 32);
	Output("CR0: ");
	Output(Chars);
	uintToHexChars(get_cr2(), Chars, 32);
	Output("\nCR2: ");
	Output(Chars);
	uintToHexChars(get_cr3(), Chars, 32);
	Output("\nCR3: ");
	Output(Chars);
	uintToHexChars(get_cr4(), Chars, 32);
	Output("\nCR4: ");
	Output(Chars);
}

void cmd_Page(char* args)
{
	char Chars[32] = {0};	
	Output("-Page Entry-\nDirectory: ");
	int address = charsToInt(args);
	//int table_index = address/4096;
	//int page_index = address/1024%4;
	
	uintToHexChars((get_cr3() & 0xFFFFF000), Chars, 32);
	Output(Chars);
	/*if (table_index > 0x400 || page_index > 0x400)
	{
		Output("] Address out of range of identity mapping.");
		return;
	}*/
	int entry = Get_PTE(address);
	Output("\nTable Index: 0x");
	uintToHexChars(table_index(address), Chars, 32); //Which Table
	Output(Chars);
	Output("\nPage Index: 0x");
	uintToHexChars(page_index(address), Chars, 32); //Which Page
	Output(Chars);
	if (entry == 0) Output("Not Mapped");
	else
	{
		Output("\nMapped To: 0x");
		uintToHexChars(entry & 0xFFFFF000, Chars, 32);
		Output(Chars);
	}
}

/*void cmd_Check(char* args)
{
	int address = charsToInt(args);
	Output(args);
	Output(" is ");
	if (vmm_check(address))
	{
		Output("free.");
	}
	else Output("taken.");
}

void cmd_Free(char* args)
{
	int address = charsToInt(args);
	vmm_free(address);
	Output(args);
	Output(" has been freed.");
}*/

void cmd_List(char* args)
{
	uintptr_t ykfs = charsToInt(args);
	if (!ykfs_check_format(ykfs)) 
	{
		if (!ykfs_check_format(current))
		{
			Output("\nInvalid filesystem format.");
			return;		
		}
		else ykfs = current;
	}
	/*ykfs_header_t* header = (ykfs_header_t*)ykfs;
	size_t variable_size = header->format.FatEntryVariableSize;
	uint32_t* entries = (uint32_t*)ykfs_get_entries(ykfs);*/
	/*for (int i = 0; i < header->format.EntryCount * 3; i+=3)
	{
		if (entries[i+2] > 0)
			Output("\n%s/%x/%d", entries[i], entries[i+1], entries[i+2]);
	}*/
	
	/*for (uint32_t* address = entries; address < (entries + header->format.Length); address += header->format.EntrySize)
	{
		uint32_t size = *(uint32_t*)(address + header->format.EntrySize);
		if (size > 0)
			Output("\n%s - %d Bytes", (char*)address, size);
	}*/
	Output("\nFiles at 0x%x", ykfs);
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	size_t variable_size = header->format.FatEntryVariableSize;
	uint32_t* entries = (uint32_t*)ykfs_get_entries(ykfs);
	size_t entry_size = header->format.EntrySize;
	size_t entry_count = header->format.EntryCount;
	int file_count = 0;
	size_t space_used = 0;
	size_t space_total = (header->format.Length) - (entry_count * entry_size);
	for (int i = 0; i < entry_count; i++)
	//for (uint32_t addr = entries; addr < (entries + header->format.Length); addr+=3)
	{
		char* name = (char*)entries[i * 3 + i];
		//uintptr_t socket = entries;
		uint32_t address = entries[i * 3 + i + 2];
		uint32_t filesize = entries[i * 3 + i +3];
		//entries += entry_size;	
		if (filesize > 0 && address > 0)
		{
			Output("\n0x%x: %s - 0x%x - %d Bytes", &entries[i * 3 + i], name, address, filesize);
			space_used += filesize;
			file_count++;
		}	
	}
	Output("\n%d/%d Files", file_count, entry_count);
	Output("\n%d/%d Bytes Used", space_used, space_total);
}

void cmd_Read(char* args, char mode)
{
	ykfs_header_t* header = (ykfs_header_t*)current;
	size_t variable_size = header->format.FatEntryVariableSize;
	uint32_t* entry = (uint32_t*)ykfs_find_entry(current, args);
	if (entry == 0) 
	{
		Output("\nFile not found.");
		return;
	}
	char* address = (char*)*((uintptr_t*)(entry + variable_size * 2));
	size_t size = *((uint32_t*)(entry + variable_size * 3));
	Output("\n");
	size_t increment = 0;
	if (mode == 'b') increment = sizeof(uint32_t);
	else if (mode == 't') increment = sizeof(char);
	for (uintptr_t* i = (uintptr_t*)address; i <= (uintptr_t*)(address + size); i += increment)
	{
		if (mode == 'b')
		{
			uint32_t n = *(uint32_t*)i;
			Output("%x", n);
		}
		else if (mode == 't')
		{
			Output("%s", i);
		}
	}
	Output("\nFile %s is at 0x%x and is %d bytes long.", args, address, size);
}