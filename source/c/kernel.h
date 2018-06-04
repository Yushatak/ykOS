/*
Kernel Header (kernel.h)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This provides a few type declarations, static translation/lookup tables, and declarations
for kernel functions.
*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include "idt.h"

//Variables
bool ring_init;
uintptr_t current;
char prompt[79];

//Function Declarations
uint32_t charsToInt(char* in);
bool StringCompare(char* buffer1, char* buffer2);
bool StartsWith(char* string, char* substring);
void OutputToPort(char *source, int port);
void OutputLengthToPort(char *source, int port, int length);
void memCopyRange(char *source, char *dest, int length);
char NibbleToChar(int nibble);
void uintToHexChars(unsigned int val, char* out, size_t len);
void uintToPrefixedHexChars(unsigned int val, char* out, size_t len);
void uintToDecChars(unsigned int val, char* out, size_t len);
void uintTo2PaddedDecChars(unsigned int val, char* out, size_t len);
void intToDecChars(int val, char* out, size_t len);
void memFill(void* address, int length, uint8_t value);
void memFillW(void* address, int length, uint16_t value);
void memFillD(void* address, int length, uint32_t value);
void mem_fill(uint8_t* address, size_t size, uint8_t value);
void memFillLength(unsigned char *dest, int length, unsigned char val);
void streamToHex(void* stream, void* hex, size_t limit);
void ClearString(char* string, size_t length);
void panic();
void WaitKey();
int GetMemoryCount();
void kernel_loop();
void TestFunction();
void cli();
void sti();
int getMemSize();
uint8_t cmos_read(uint8_t reg);
void CommandParser();
void pad_left(char target[], char c, int length);
void pad_right(char target[], char c, int length);
int strlen(char* target);

//Handlers
void Interrupt(isr_registers_t* regs);
void KeyboardHandler(isr_registers_t* regs);
void PageFaultHandler(isr_registers_t* regs);
void DumpRegisters(isr_registers_t* regs);
void TimerHandler(isr_registers_t* regs);