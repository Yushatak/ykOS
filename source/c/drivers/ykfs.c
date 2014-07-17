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
	header->format.FatEntryVariableSize = fat_width / 8;
	header->format.ClusterSize = cluster_size;
	header->format.ReservedSectors = reserved_sector_count;
	header->format.Length = size_in_bytes - sizeof(ykfs_header_t) - (512*reserved_sector_count);
	header->format.EntryCount = header->format.Length / header->format.FatEntryVariableSize / 3;
	header->format.EntrySize = fat_width / 8 * 3;
	ykfs_wipe_entries(base);
}

void ykfs_wipe_entries(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	mem_fill((uint8_t*)ykfs_get_entries(ykfs), header->format.EntryCount * header->format.EntrySize, 0);
}

uintptr_t ykfs_get_entries(uintptr_t ykfs)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
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
	uintptr_t entries = ykfs_get_entries(ykfs);
	size_t entry_size = header->format.EntrySize;
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
	uintptr_t entries = ykfs_get_entries(ykfs);
	size_t variable_size = header->format.FatEntryVariableSize;
	size_t entry_size = header->format.EntrySize;
	size_t entry_count = header->format.EntryCount;
	for (int i = 0; i < entry_count; i++)
	{
		if (*(size_t*)(entries + variable_size * 2) == 0) return entries;
		else entries += entry_size;
		//Need to make this check for the end of the FAT and return an error if exceeded.
	}
	return 0;
}

void ykfs_new_file(uintptr_t ykfs, uintptr_t entry, char* name, uintptr_t address, size_t size)
{
	ykfs_header_t* header = (ykfs_header_t*)ykfs;
	if (!ykfs_check_format(ykfs))
	{
		Output("\nInvalid filesystem format.");
		return;
	}
	size_t variable_size = header->format.FatEntryVariableSize;
	memCopyRange(name, (char*)entry, sizeof(name));
	*(uintptr_t*)(entry + variable_size) = address;
	*(size_t*)(entry + variable_size * 2) = size;
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