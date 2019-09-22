/*
Ramdisk Driver Header (ramdisk.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

Simple ramdisk driver header.
*/

//Function Declarations
uintptr_t get_ramdisk(size_t size_in_bytes/*, uint32_t fat_width*/);
void discard_ramdisk(uintptr_t ramdisk);