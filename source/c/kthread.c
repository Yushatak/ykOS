/*
Kernel Threading (kthread.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the code for kernel threading.
*/

#include "kthread.h"
#include "kernel.h"

//Externs
extern void* kernel_end;
extern void* stack_start;
extern void ctxt_sw(void** oldStack, void* newStack);

void construct_boot_kthread(kthread_t* store_at)
{
	kthread_t* kt = store_at;
	kt->stack_top = (uint32_t)&kernel_end; //Stack goes from stack_start to kernel_end
	//kt->stack_base = (uint32_t)&stack_start;
	kt->entry_point = 0x100000;
}

void get_kthread(void* store_at, uint32_t entry_point)
{
	kthread_t* kt = (kthread_t*)store_at;
	kt->entry_point = entry_point;
	kt->stack_top = 0x1FFFF; //Temporary placeholder, no physical memory manager yet.
	uint32_t* sp = (uint32_t*)0x1FFFF - sizeof(uint32_t);
	*sp = entry_point;
	for (int i = 0; i < 8; i++) //zero out register values and adjust stack pointer
	{
		sp--;
		*sp = 0;
	}
	kt->stack_pointer = (uint32_t)sp;
}

void switch_kthread(kthread_t* ckt, kthread_t* kt)
{
	ctxt_sw((void**)&ckt->stack_pointer, (void*)kt->stack_pointer);
}