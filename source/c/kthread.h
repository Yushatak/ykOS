/*
Kernel Threading Header (kthread.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the struct/function declarationsfor kernel threading.
*/

//Structs
typedef struct kthread
{
	char name[32];
	uint16_t thread_id;
	uint32_t entry_point;
	uint32_t stack_base, stack_pointer, stack_end;
	uint32_t eax, ebx, ecx, edx, ebp, edi, esi;
	uint8_t priority;	
} kthread_t;

//Function Declarations
kthread_t* construct_boot_kthread(void* store_at);
kthread_t* get_kthread(void* store_at, void* entry_point, uint16_t stack_size);
void switch_kthread(kthread_t* kt);