/*
Thread Scheduler Header (scheduler.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
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
	uint32_t stack_top, stack_pointer, entry_point;
	uint32_t page_table;
	bool sleeping;
	uint32_t ticks;
	uint8_t ring;
	void* previous_thread;
	void* next_thread;
	bool interrupt_state;
} thread_t;

typedef struct ring
{
	uint8_t rid;
	uint32_t thread_count;
	thread_t* top_thread;
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