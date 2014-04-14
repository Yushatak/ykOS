/*
Inline (inline.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Provides inline functions.
*/
#include <stdint.h>
#include "inline.h"

inline void untracked_sti()
{
	__asm__ volatile("sti");
}

inline void untracked_cli()
{
	__asm__ volatile("cli");
}

inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile ("outb %1, %0" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile ("inb %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}

inline void halt()
{
	__asm__ volatile("hlt");
}

inline uint32_t get_eax()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, eax" : "=b"(ret));
	return ret;
}

inline uint32_t get_ebx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, ebx" : "=a"(ret));
	return ret;
}

inline uint32_t get_ecx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, eax" : "=a"(ret));
	return ret;
}

inline uint32_t get_edx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, edx" : "=a"(ret));
	return ret;
}

inline uint32_t get_esp()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, esp" : "=a"(ret));
	return ret;
}

inline uint32_t get_ebp()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, ebp" : "=a"(ret));
	return ret;
}

inline uint32_t get_esi()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, esi" : "=a"(ret));
	return ret;
}

inline uint32_t get_edi()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, edi" : "=a"(ret));
	return ret;
}

inline uint32_t get_cr0()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr0" : "=a"(ret));
	return ret;
}

inline uint32_t get_cr2()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr2" : "=a"(ret));
	return ret;
}
	
inline uint32_t get_cr3()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr3" : "=a"(ret));
	return ret;
}
	
inline uint32_t get_cr4()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr4" : "=a"(ret));
	return ret;
}
	
/*inline uint64_t get_cr8()
{
	uint64_t ret = 0;
	__asm__ volatile ("mov %0, cr8" : "=a"(ret));
	return ret;
}*/