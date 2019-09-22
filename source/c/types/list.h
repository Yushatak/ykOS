/*
List Header (List.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
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