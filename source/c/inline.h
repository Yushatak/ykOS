/*
Inline Header (inline.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

Provides function declarations for inline functions.
*/

void outb(uint16_t port, uint8_t val);
uint8_t inb(uint16_t port);
uint32_t get_eax();
uint32_t get_ebx();
uint32_t get_ecx();
uint32_t get_edx();
uint32_t get_esp();
uint32_t get_ebp();
uint32_t get_esi();
uint32_t get_edi();
uint32_t get_cr0();
uint32_t get_cr2();
uint32_t get_cr3();
uint32_t get_cr4();
//uint32_t get_cr8();
void halt();
void untracked_cli();
void untracked_sti();