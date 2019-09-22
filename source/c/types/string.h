/*
String Header (string.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

A struct for representing strings and functions that operate on that struct.
*/

typedef struct string
{
	char[] value;
	size_t length;
} string_t;