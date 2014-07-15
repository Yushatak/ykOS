/*
Screen (screen.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This provides functions and variables for interaction with a 80x25 text-mode screen.
*/
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "kernel.h"
#include "screen.h"
#include "inline.h"

const int VGABaseAddress = 0xB8000;
const int VGALimit = 0xBFFFF;
const int VGAControlBase = 0x3D4;

void initialize_screen()
{
	statusLine = 0;
	promptLine = 24;
	CursorX = 0;
	CursorY = 23;
	ScreenColumns = 80;
	ScreenRows = 25;
	BytesPerChar = 2;
	scroll = true;
	vga_memory = (uint16_t*)0xB8000;
	ClearScreen();
}

void SetCursor(int x, int y)
{
	unsigned short pos = (y * ScreenColumns) + x;
	outb(VGAControlBase, 0x0F);
	outb(VGAControlBase + 1, (unsigned char)(pos & 0xFF));
	outb(VGAControlBase, 0x0E);
	outb(VGAControlBase + 1, (unsigned char)((pos >> 8) & 0xFF));
}

void ClearLine(int y)
{
	for (int x = 0; x <  80; x++)
	{
		vga_memory[ScreenColumns * y + x] = 0x0720;
	}
}

void Scroll()
{
	for (int y = statusLine + 2; y <= promptLine; y++)
	{
		for (int x = 0; x <  ScreenColumns; x++)
		{
			vga_memory[ScreenColumns * (y - 1) + x] = vga_memory[ScreenColumns * y + x];
		}
	}
	CursorX = 0; //Line Feed
}
void OutputToPort(char *source, int port)
{
	while (*source)
	{
		outb(port, *source);
		source++;
	}
}

void OutputLengthToPort(char *source, int port, int length)
{
	int i = 0;
	while (*source)
	{
		if (i < length)
		{
			outb(port, *source);
			source++;
			i++;
		}
		else return;
	}
}

void Output(const char* source, ...)
{
	va_list args;
	va_start(args, source);
	base_Output(source, args);
	va_end(args);
}

void OutputLine(int line, const char *source, ...)
{
	va_list args;
	va_start(args, source);
	ClearLine(line);
	int old_x = CursorX;
	int old_y = CursorY;
	CursorX = 0;
	CursorY = line;
	scroll = false;
	base_Output(source, args);
	scroll = true;
	CursorX = old_x;
	CursorY = old_y;
	va_end(args);
}

void base_Output(const char *source, va_list args)
{
	char c;
	char buffer[32] = {0};
	while ((c = *source++) != 0)
	{
		if (CursorX == ScreenColumns - 1 && scroll) Scroll();
		if (c == '%')
		{
			char* ptr;
			c = *source++;
			switch (c)
			{
				case 'd':
					intToDecChars(va_arg(args, int), buffer, 32);
					ptr=buffer;
					goto string;
					break;
				case 'u':
					uintToDecChars(va_arg(args, unsigned int), buffer, 32);
					ptr = buffer;
					goto string;
					break;
				case '2':
					uintTo2PaddedDecChars(va_arg(args, unsigned int), buffer, 32);
					ptr = buffer;
					goto string;
					break;
				case 'h':
				case 'x':
					uintToHexChars(va_arg(args, unsigned int), buffer, 32);
					ptr=buffer;
					goto string;
					break;
				case 's':
					ptr=va_arg(args, char*);
					if (!ptr)
					{
						buffer[0] = 'N';
						buffer[1] = 'U';
						buffer[2] = 'L';
						buffer[3] = 'L';
						buffer[4] = 0;
						ptr = buffer;
					}
					goto string;
					break;
				case '%':
					OutputChar('%');
					break;
				string:
					while(*ptr)
						OutputChar(*ptr++);
					break;
				default:
					//OutputChar(*((int*)arg++));
					break;
			}
		}
		else OutputChar(c);
	}
}

void OutputChar(char c)
{
	if (c == '\n')
	{
		Scroll();
		return;
	}
	char *destination = (char *)GetVideoAddress(CursorX++, CursorY);
	*destination = c;
	*++destination = 0x07;
}

//Outputs the string at the address pointed to by *source to the screen coordinates specified, advancing the text cursor when done.
void OutputAt(char *source, int x, int y) 
{
	char *destination = (char *)GetVideoAddress(x, y);
	int i = 0;
	while (*source) 
	{
		i++;
		if (*destination + BytesPerChar >= VGALimit) return;
		*destination++ = *source++;
		*destination++ = 7; 
	}
}

void OutputCharAt(char c, int x, int y) 
{
	*(char *)GetVideoAddress(x, y) = c;
}

void OutputHexByteAt(uint8_t byte, int x, int y)
{
	char Chars[3];
	uintToHexChars(byte, Chars, 3);
	Chars[2] = 0x0; //terminator
	OutputAt(Chars, x, y);
}

void OutputHexByte(uint8_t byte)
{
	char Chars[3];
	uintToHexChars(byte, Chars, 3);
	Chars[2] = 0x0; //terminator
	Output(Chars);
}

void ClearScreen()
{
	for (int y = 0; y < 25; y++)
	{
		for (int x = 0; x < 80; x++)
		{
			vga_memory[ScreenColumns * y + x] = 0x0720;
		}
	}
	//memFillW((void*)VGABaseAddress, VGALimit - VGABaseAddress, 0x20 | (((0 << 4) | (15 & 0x0F)) << 8));	
}

//Based on variables (defaults for 80x25 VGA text mode) calculates the video memory offset that equates to an X,Y coordinate in chars.
void* GetVideoAddress(int x, int y)
{
	return (void*)((ScreenColumns * y + x) * BytesPerChar + VGABaseAddress);
}