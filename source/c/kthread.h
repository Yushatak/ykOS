/*
Kernel Threading Header (kthread.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the struct/function declarationsfor kernel threading.
*/
#include <stdint.h>

//Structs
typedef struct kthread
{
	uint32_t entry_point, stack_top, stack_pointer;
} kthread_t;

//Function Declarations
void construct_boot_kthread(kthread_t* store_at);
void get_kthread(void* store_at, uint32_t entry_point);
void switch_kthread(kthread_t* ckt, kthread_t* kt);