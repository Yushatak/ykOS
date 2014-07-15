/*
ykFS Driver Header (ykfs.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Yushatak filesystem (ykFS) driver header.
*/

#include <stdint.h>
#include <stdbool.h>

//Structs
typedef struct ykfs_format_info
{
	uint16_t reserved; //JMP instruction
	char magic_1; //Y
	uint16_t FatEntryVariableSize;
	char magic_2; //K
	uint32_t ClusterSize;
	char magic_3; //F
	uint32_t ReservedSectors;
	char magic_4; //S
	uint32_t Length;
	uint32_t EntryCount;
} ykfs_format_info_t;

typedef struct ykfs_header
{
	uint8_t boot_sector[512];
	ykfs_format_info_t format;
	uint8_t reserved[20];
} ykfs_header_t;

//Function Declarations
uintptr_t ykfs_get_entries(uintptr_t ykfs);
uintptr_t ykfs_find_entry(uintptr_t ykfs, const char* name);
void ykfs_wipe_entries(uintptr_t ykfs);
void ykfs_format_memory(uintptr_t base, size_t size_in_bytes, size_t cluster_size, size_t fat_width, uint32_t reserved_sector_count);
bool ykfs_check_format(uintptr_t ykfs);