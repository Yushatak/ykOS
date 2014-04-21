/*
ykFS Driver (ykfs.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Yushatak filesystem (ykFS) driver.
*/

#include <stdint.h>
#include "../memory.h"
#include "ramdisk.h"
#include "ykfs.h"
#include "../kernel.h"
#include "../screen.h"

uintptr_t ykfs_get_entries(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	size_t variable_size = header->format.FatEntryVariableSize;
	uintptr_t entries = ykfs + sizeof(ykfs_header_t);
	return entries;
}

uintptr_t ykfs_find_entry(uintptr_t ykfs, const char* name)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	size_t variable_size = header->format.FatEntryVariableSize;
	uintptr_t entries = ykfs_get_entries(ykfs);
	uintptr_t page = entries;
			Output("\nChecking at 0x%x...", entries);
	size_t entry_size = variable_size * 3;
	size_t entry_count = ((4096 - sizeof(ykfs_header_t))/variable_size/3);
	int i = 0;
	//while (((char*)entries)[0] != name[0])
	while (!StringCompare((char*)entries, (char*)name))
	{
		entries += entry_size;
		if (i >= page + 4096) 
		{
			entries = *(uint32_t*)page;
			page = entries;
			Output("\nChecking at 0x%x...", entries);
		}
	}
	return (uintptr_t)entries;
}