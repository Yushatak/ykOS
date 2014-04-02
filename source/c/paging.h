/*
Paging Header (paging.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains declarations for the functions to set up and manipulate page tables.
*/

//Constants
const static int page_size = 0x1000;
const static int entry_size = 32768;

//Structs
typedef struct page_table
{
	uint32_t entries[1024];
} page_table_t;

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