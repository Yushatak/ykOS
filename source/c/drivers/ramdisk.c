/*
Ramdisk Driver (ramdisk.c)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

Simple ramdisk driver.
*/

#include <stdint.h>
#include "../memory.h"
#include "ykfs.h"
#include "../kernel.h"
#include "../screen.h"

uintptr_t get_ramdisk(size_t size_in_bytes/*, size_t fat_width*/)
{
	uintptr_t ramdisk = get_linked_pages(size_in_bytes);
	ykfs_format_memory(ramdisk, size_in_bytes, 4096/*, fat_width*/, 0);
	return ramdisk;
}

void discard_ramdisk(uintptr_t ramdisk)
{
	free_linked_pages(ramdisk);
}