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

//Function Declarations
uint32_t* get_address_space(size_t size_in_bytes);

//Inline