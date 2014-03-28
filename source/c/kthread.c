/*
Kernel Threading (kthread.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the code for kernel threading, depending heavily on "context.c".
*/

#include "context.h"
#include "kthread.h"

//Externs
extern void* kernel_end;
extern void* stack_start;

kthread_t* construct_boot_kthread(kthread_t* store_at)
{
	kthread_t* kt = store_at;
	kt->stack_end = &kernel_end; //Stack goes from stack_start to kernel_end
	kt->stack_pointer = get_esp();
	kt->stack_base = &stack_start;
	kt->entry_point = get_eip();
	return kt;
}

kthread_t* get_kthread(void* store_at, void* entry_point, uint16_t stack_size)
{
	kthread_t* kt = (kthread_t*)store_at;
	kt->entry_point = &entry_point;
	kt->priority = 0;
	kt->stack_base = 0x400000; //Temporary placeholder, no physical memory manager yet.
	kt->stack_end = kt->stack_base - stack_size; //Auto-calculate the stack end based on base/size.
	kt->stack_pointer = kt->stack_end; //Start at the top, this is a new thread.
	return kt;
}

void switch_kthread(kthread_t* kt)
{
	context_switch((void*)get_esp(), &kt->stack_pointer);
}