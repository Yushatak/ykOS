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

//Note: this function does *not* care what is in memory, it is up to you to make sure it is safe to call it with given parameters.
void ykfs_format_memory(uintptr_t base, size_t size_in_bytes, size_t cluster_size, size_t fat_width, uint32_t reserved_sector_count)
{
	ykfs_header_t* header = (ykfs_header_t*)base;
	uintptr_t ykfs_end = base + size_in_bytes;
	header->format.FatEntryVariableSize = fat_width/8;
	header->format.ClusterSize = cluster_size;
	header->format.ReservedSectors = reserved_sector_count;
	uintptr_t ykfs_entries = ykfs_get_entries(base);
}

void ykfs_wipe_entries(uintptr_t ykfs)
{
	uintptr_t entries = ykfs_get_entries(ykfs);
	
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
	size_t variable_size = header->format.FatEntryVariableSize;
	uintptr_t entries = ykfs_get_entries(ykfs);
	uintptr_t page = entries;
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
		}
	}
	return (uintptr_t)entries;
}