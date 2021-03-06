/*
Thread Scheduler Header (scheduler.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

Functions for managing thread creation, manipulation, and scheduling.
*/

#include <stdint.h>
#include <stdbool.h>

//Structs
typedef struct thread
{
	uint16_t tid;
	uint8_t priority;
	uint32_t stack_top, stack_pointer;
	uint32_t entry_point;
	uint32_t page_table;
	bool sleeping;
	unsigned long ticks;
	int8_t ring;
	void* previous_thread;
	void* next_thread;
	bool interrupt_state;
} thread_t;

typedef struct ring
{
	uint8_t rid;
	uint32_t thread_count;
	thread_t* top_thread; //Need to figure out my intended meaning for this and document it - I imagine it's like a "root" or "primary" thread for a ring.
} ring_t;

//Typedefs
typedef void (*entry_point_t)(void);

//Variable Declarations
ring_t** ring;
thread_t* current_thread;

//Function Declarations
void initialize_rings();
void initialize_thread(thread_t* t);
void next_thread();
void sleep_thread(thread_t* t);
void wake_thread(thread_t* t);
void bounce();
void assign_ring(thread_t* t, ring_t* r);