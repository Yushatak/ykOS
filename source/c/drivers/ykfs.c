/*
ykFS Driver (ykfs.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Yushatak filesystem (ykFS) driver.
*/

#include <stdint.h>
#include <stdbool.h>
#include "../memory.h"
#include "ramdisk.h"
#include "ykfs.h"
#include "../kernel.h"
#include "../screen.h"

//Note: this function does *not* care what is in memory, it is up to you to make sure it is safe to call it with given parameters.
void ykfs_format_memory(uintptr_t base, size_t size_in_bytes, size_t cluster_size, size_t fat_width, uint32_t reserved_sector_count)
{
	ykfs_header_t* header = (ykfs_header_t*)base;
	header->format.magic_1 = 'Y';
        header->format.magic_2 = 'K';
        header->format.magic_3 = 'F';
	header->format.magic_4 = 'S';
	header->format.FatEntryVariableSize = fat_width/8;
	header->format.ClusterSize = cluster_size;
	header->format.ReservedSectors = reserved_sector_count;
	header->format.Length = size_in_bytes - sizeof(ykfs_header_t) - (512*reserved_sector_count);
	header->format.EntryCount = header->format.Length / header->format.FatEntryVariableSize / 3;
	ykfs_wipe_entries(base);
}

void ykfs_wipe_entries(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	uintptr_t entries = ykfs_get_entries(ykfs);
	memFillD((void*)entries, header->format.EntryCount * header->format.FatEntryVariableSize * 3, 0);
}

uintptr_t ykfs_get_entries(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	size_t variable_size = header->format.FatEntryVariableSize;
	size_t reserved_sectors = header->format.ReservedSectors;
	//Entries start after the header and any reserved sectors (512 bytes per sector).
	uintptr_t entries = ykfs + sizeof(ykfs_header_t) + (512*reserved_sectors);
	return entries;
}

uintptr_t ykfs_find_entry(uintptr_t ykfs, const char* name)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	if (!ykfs_check_format(ykfs))
	{
		Output("\nInvalid filesystem format.");
		return 0;
	}
	size_t variable_size = header->format.FatEntryVariableSize;
	uintptr_t entries = ykfs_get_entries(ykfs);
	size_t entry_size = variable_size * 3 / 8;
	size_t entry_count = header->format.EntryCount;
	for (int i = 0; i < entry_count; i++)
	{
		if (StringCompare((char*)entries, (char*)name))
			return entries;
		else
			entries += entry_size;
	}
	Output("Result: 0x%x", entries);
	if (StringCompare((char*)entries, (char*)name))
		return entries;
	else return 0;
}

uintptr_t ykfs_next_empty(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	if (!ykfs_check_format(ykfs))
	{
		Output("\nInvalid filesystem format.");
		return 0;
	}
	size_t variable_size = header->format.FatEntryVariableSize;
	uintptr_t entries = ykfs_get_entries(ykfs);
	//Not sure why its wonky here but these are the right values.
	size_t entry_size = variable_size / 2 * 3; 
	size_t entry_count = header->format.EntryCount;
	for (int i = 0; i < entry_count; i++)
	{
		//Output("\n0x%x + 0x%x = 0x%x", entries, variable_size, entries + variable_size);
		uintptr_t filesize = entries + variable_size;
		if (*(size_t*)filesize == 0) return entries;
		else entries += entry_size;
	}
	return 0;
}

bool ykfs_check_format(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	return (header->format.magic_1 == 'Y' &&
		   header->format.magic_2 == 'K' &&
		   header->format.magic_3 == 'F' &&
		   header->format.magic_4 == 'S'	
	);
}