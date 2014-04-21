/*
Ramdisk Driver (ramdisk.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Simple ramdisk driver.
*/

#include <stdint.h>
#include "../memory.h"
#include "ykfs.h"
#include "../kernel.h"
#include "../screen.h"

uintptr_t get_ramdisk(size_t size_in_bytes, uint32_t fat_width)
{
	uintptr_t ramdisk = get_linked_pages(size_in_bytes);
	ykfs_header_t* header = (ykfs_header_t*)ramdisk;
	header->format.FatEntryVariableSize = fat_width/8; //Bits to bytes
	header->format.ClusterSize = 4096;
	header->format.ReservedSectors = 0;
	return ramdisk;
}