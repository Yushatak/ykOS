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
extern void EnablePaging();

//Structs
typedef struct page_table
{
	uint32_t entries[1024];
} page_table_t;

//Function Declarations
uint32_t GetPageEntry(void* table, int index);
void PutPageEntry(void* table, int index, uint32_t entry);
void PutPageEntryA(int address, uint32_t entry);
uint32_t GetPageEntryA(int address);
uint32_t GetPageA(int address);
uint32_t GetEntryA(int address);