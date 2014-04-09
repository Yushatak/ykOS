/*
Thread Scheduler (scheduler.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

Functions for managing thread creation, manipulation, and scheduling.
*/

#include <stdint.h>
#include <stddef.h>
#include "kernel.h"
#include "memory.h"
#include "pmm.h"
#include "kthread.h"
#include "scheduler.h"

//Defines
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))
#define BOCHS_BP() __asm__ volatile ("xchg bx,bx");

//Externs
extern void ctxt_sw(void** oldStack, void* newStack);
extern void* stack_start;

void initialize_rings()
{
	ring[0] = (ring_t*)pmm_alloc();
	ring[0]->rid = 0;
	ring[0]->thread_count = 1;
	thread_t* boot_thread = (thread_t*)(ring[0]+(sizeof(ring_t)));
	boot_thread->ring = 0;
	boot_thread->tid = 0;
	boot_thread->priority = 0;
	boot_thread->page_table = get_cr3();
	boot_thread->ticks = 0;
	//boot_thread->stack_top = (uint32_t)&stack_start;
	boot_thread->stack_top = (uint32_t)pmm_alloc();
	boot_thread->interrupt_state = true;
	uint32_t ep = (uint32_t)&kernel_loop;
	boot_thread->entry_point = ep;
	boot_thread->eip = ep;
	boot_thread->sleeping = false;
	current_thread = boot_thread;
	initialize_thread(boot_thread);
	
	ring[0]->thread_count++;
	thread_t* test_thread = (thread_t*)(ring[0] + sizeof(ring_t) + sizeof(thread_t));
	test_thread->ring = 0;
	test_thread->tid = 1;
	test_thread->priority = 0;
	test_thread->page_table = get_cr3();
	test_thread->ticks = 0;
	test_thread->stack_top = (uint32_t)pmm_alloc();
	test_thread->interrupt_state = true;
	ep = (uint32_t)&TestFunction;
	test_thread->entry_point = ep;
	test_thread->eip = ep;
	test_thread->sleeping = false;
	initialize_thread(test_thread);
	test_thread->next_thread = boot_thread;
	test_thread->previous_thread = boot_thread;
	boot_thread->next_thread = test_thread;
	boot_thread->previous_thread = test_thread;
	
	ring[1] = (ring_t*)pmm_alloc();
	ring[1]->rid = 1;
	ring[1]->thread_count = 0;
	ring[1]->top_thread = NULL;
	
	ring[2] = (ring_t*)pmm_alloc();
	ring[2]->rid = 2;
	ring[2]->thread_count = 0;
	ring[2]->top_thread = NULL;
}

void initialize_thread(thread_t* t)
{
	uint32_t* sp = (uint32_t*)t->stack_top;
	sp--;
	*sp = (uint32_t)&bounce;
	for (int i = 0; i < 8; i++) //zero out register values and adjust stack pointer
	{
		sp--;
		*sp = 0;
	}
	t->stack_pointer = (uint32_t)sp;
}

void bounce()
{
	Output("\nBounce (#%d)", current_thread->tid);
	uint32_t* sp = (uint32_t*)current_thread->stack_top;
	sp--;
	*sp = current_thread->eip;
	__asm__ volatile("sti");
	((entry_point_t)current_thread->entry_point)();
	Output("\nThread #%d exited unexpectedly.", current_thread->tid);
	panic();
}

void assign_ring(thread_t* t, ring_t* r)
{
	if (!(t == current_thread))
	{
		//Decrement old ring if present.
		if (t->ring > -1) ring[t->ring]->thread_count--;
		//Erase the thread from the chain.
		thread_t* old_previous = (thread_t*)t->previous_thread;
		thread_t* old_next = (thread_t*)t->next_thread;
		old_previous->next_thread = old_next;
		old_next->previous_thread = old_previous;
		//Assign new ring, increment count.
		t->ring = r->rid;
		r->thread_count++;
		//Add the thread to the new chain.
		thread_t* top = ring[t->ring]->top_thread; //need to preserve old next and set as this one's next
		top->next_thread = t;
		t->previous_thread = top;
	}
	else Output("\nThread %d is active, can't change rings!");
}

void next_thread()
{		
	if (current_thread->interrupt_state)
	{
		thread_t* old_thread = current_thread;
		current_thread = current_thread->next_thread;
		if (old_thread != current_thread)
		{
			Output("\nSwitching from #%d to #%d.", old_thread->tid, current_thread->tid);
			if (old_thread->page_table != current_thread->page_table) 
			{
				__asm__ volatile ("mov cr3, %0" :: "b"(current_thread->page_table));
			}
			ctxt_sw((void**)&old_thread->stack_pointer, (void*)current_thread->stack_pointer);
		}
		else Output("\nSkipping a swap from #%d to #%d.", old_thread->tid, current_thread->tid);
	}
}

void sleep_thread(thread_t* t)
{
	t->sleeping = true;
}

void wake_thread(thread_t* t)
{
	t->sleeping = false;
}