/*
Memory Header (memory.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains declarations for the functions to set up and manipulate memory access.
*/
//External Functions
extern void A20();
extern void LinearGDT();
extern void enablePaging();

//Structs
typedef struct page_table
{
	uint32_t entries[1024];
} page_table_t;

//From http://wiki.osdev.org/Detecting_Memory_(x86)
typedef struct multiboot_memory_map {
	uint32_t size;
	uint32_t base_addr_low,base_addr_high;
	uint32_t length_low,length_high;
	uint32_t type;
} multiboot_memory_map_t;

//Function Declarations
void EnablePaging();
uint32_t Get_PTE(uint32_t vaddr);
void Set_PTE(uint32_t vaddr, uint32_t value);
void Map_PTE(uint32_t vaddr, uint32_t paddr, uint32_t flags);
void Set_Flags_PTE(uint32_t vaddr, uint32_t flags);
void Enable_PTE(uint32_t vaddr);
void Disable_PTE(uint32_t vaddr);
void Mirror_PTE(uint32_t vaddr, uint32_t vaddr2);

//Inline
void invlpg(uint32_t vaddr);
void reload_cr3();
uint32_t table_index(uint32_t vaddr);
uint32_t page_index(uint32_t vaddr);