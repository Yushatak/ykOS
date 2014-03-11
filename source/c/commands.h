/*
Commands Header (commands.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This contains the command definitions for individual built-in command applets in the kernel 
shell.
*/
#include <stdint.h>
#include <stdbool.h>

void cmd_Peek(char* args, size_t argsize);
void cmd_Convert(char* args);
void cmd_Creg(void);
void cmd_Page(char* args);