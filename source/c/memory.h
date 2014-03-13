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

//Function Declarations
void EnablePaging();
uint32_t Get_ID_PTE(uint32_t vaddr);
void Set_ID_PTE(uint32_t vaddr, uint32_t value);

//Inline
void invlpg(uint32_t vaddr);
void reload_cr3();
uint32_t table_index(uint32_t vaddr);
uint32_t page_index(uint32_t vaddr);