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
#include "commands.h"
#include "memory.h"

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
	intToChars(*(uint32_t *)charsToInt(args), Chars);
	Output(Chars);
}

void cmd_Creg(void)
{
	char Chars[32] = {0};
	intToChars(get_cr0(), Chars);
	Output("CR0: ");
	Output(Chars);
	intToChars(get_cr2(), Chars);
	Output("\nCR2: ");
	Output(Chars);
	intToChars(get_cr3(), Chars);
	Output("\nCR3: ");
	Output(Chars);
	intToChars(get_cr4(), Chars);
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
	
	intToChars((get_cr3() & 0xFFFFF000), Chars);
	Output(Chars);
	ClearString(Chars, 32);
	/*if (table_index > 0x400 || page_index > 0x400)
	{
		Output("] Address out of range of identity mapping.");
		return;
	}*/
	int entry = Get_PTE(address);
	Output("\nTable Index: 0x");
	intToChars(table_index(address), Chars); //Which Table
	Output(Chars);
	Output("\nPage Index: 0x");
	ClearString(Chars, 32);
	intToChars(page_index(address), Chars); //Which Page
	Output(Chars);
	if (entry == 0) Output("Not Mapped");
	else
	{
		Output("\nMapped To: 0x");
		ClearString(Chars, 32);
		intToChars(entry & 0xFFFFF000, Chars);
		Output(Chars);
	}
}