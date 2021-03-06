/*
Commands Header (commands.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
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
//void cmd_Check(char* args);
//void cmd_Free(char* args);
void cmd_List(char* args);
void cmd_Read(char* args, char mode);