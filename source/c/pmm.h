/*
Physical Memory Manager Header (pmm.h)
Part of the ykOS Project
Written by Johnathan "Yushatak" Schur
Copyright Yushatak 2014-2018
All Rights Reserved

This contains declarations for the functions to manage and provide allocation/deallocation functionality for virtual memory.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//Variables
extern uint32_t free_pages;
uint32_t* current_page;

//Function Definitions
void pmm_free(void* page);
uint32_t* pmm_alloc();
void pmm_claim(uint32_t* address, size_t size);