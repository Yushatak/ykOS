/*
List Header (List.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

List types, structs, and operations.
*/

typedef struct list_item
{
	list_item *previous, *next;
	void* data;
} list_item_t;

typedef struct list
{
	list_item *first, *last;
	size_t count;
} list_t;