/*
Kernel Threading Header (kthread.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the struct/function declarationsfor kernel threading.
*/

typedef struct kthread
{
	char name[32];
	uint16_t thread_id;
	uint32_t stack_base, stack_pointer;
	uint8_t priority;	
} kthread_t;