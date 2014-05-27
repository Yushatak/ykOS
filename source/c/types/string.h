/*
String Header (string.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

A struct for representing strings and functions that operate on that struct.
*/

typedef struct string
{
	char[] value;
	size_t length;
} string_t;