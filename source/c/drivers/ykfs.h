/*
ykFS Driver Header (ykfs.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Yushatak filesystem (ykFS) driver header.
*/

#include <stdint.h>

//Structs
typedef struct ykfs_format_info
{
	uint16_t reserved; //JMP instruction
	uint16_t FatEntryVariableSize;
	uint32_t ClusterSize;
	uint32_t ReservedSectors;
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