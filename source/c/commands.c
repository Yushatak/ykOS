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
#include "paging.h"
#include "commands.h"
#include "memory.h"
#include "vmm.h"

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