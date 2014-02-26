#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "kernel.h"
#include "commands.h"
#include "idt.h"

//Kernel Memory Map
/*

0x07C0-0x07E0 - Bootloader/Bootloader Data (including GDT)
0x2000+ - Kernel/Kernel Data

*/

//String Declarations
char message[] = "32-Bit Kernel Loaded";
char prompt[] = "ykOS>";

//Buffers
char keybuffer[256] = {0};
int keybuffer_ptr = 0;
char cmdbuffer[256] = {0};

//Pointers
uint8_t* memory = (uint8_t*)0x0;
uint16_t* vga_memory = (uint16_t*)0xB8000;

//Other Variable Declarations
int CursorX = 0;
int CursorY = 23;
int ScreenColumns = 80;
int ScreenRows = 25;
const int VGABaseAddress = 0xB8000;
const int VGALimit = 0xBFFFF;
int BytesPerChar = 2;
const int VGAControlBase = 0x3D4;
isr_t intHandlers[256];
bool shift = false;
bool ctrl = false;
bool alt = false;
int promptLine = 24;

//Entry Point (this must remain the first function!)
int main(void)
{		
	//Remap PIC IRQ Table 0->32
	outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 0x28);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x00);
	outb(0xA1, 0x00);
	
	//Generate IDT
	generateIDT();
	ClearScreen();
	registerISR(0x21, &KeyboardHandler);
	OutputAt(prompt, 0, promptLine);
	SetCursor(sizeof(prompt) - 1, promptLine);
	
	//Loop forever until interrupted.
	for(;;);
}

void Interrupt(isr_registers_t* regs)
{	
	if (regs->intvec >= 0x28) outb(0xA0, 0x20); //Reset Slave PIC
	outb(0x20, 0x20); //Reset Master PIC
	
	if (intHandlers[regs->intvec] != 0)
	{
		((isr_t)intHandlers[regs->intvec])(regs);
	}
	
	//if (regs.intvec >= 0x20 && regs.intvec <= 0x2F)
	//{
		/*This is an IRQ*/
		//Output(irqLabel, 0, 24);
		//OutputHexByte(regs.intvec, 4, 24);
	//}
	//else
	//{
		/*This is an ISR*/
		//Output(isrLabel, 0, 23);
		//OutputHexByte(regs.intvec, 4, 23);
	//}
}

void ClearLine(int y)
{
	for (int x = 0; x <  80; x++)
	{
		vga_memory[ScreenColumns * y + x] = 0x0720;
	}
}

void CommandParser()
{
	char* address = cmdbuffer;
	char* splitPos = 0;
	while (*address)
	{
		//Set splitPos to the address after the first space found in the command string, to denote the split where args begin.
		if (*address++ == ' ') 
		{
			splitPos = address;
			//If there's a space at the end of the string, this doesn't count as args.
			if (*address + 1 == 0) splitPos = 0;
			break;
		}
	}
	if (StartsWith(cmdbuffer, "peek"))
	{
		if (splitPos == 0) Output("Invalid Argument(s).\n");
		else cmd_Peek(splitPos, (keybuffer_size - (int)splitPos) + keybuffer_ptr);
	}
	else if (StartsWith(cmdbuffer, "convert"))
	{
		if (splitPos == 0) Output("Invalid Argument(s).\n");
		else cmd_Convert(splitPos);
	}
	else if (StringCompare(cmdbuffer, "clear") || StringCompare(cmdbuffer, "cls"))
	{
		ClearScreen();
		OutputAt(prompt, 0, promptLine);
		SetCursor(sizeof(prompt) - 1, promptLine);
		CursorX = 0;
	}
	else 
	{
		Output("Invalid Command.\n");
	}
	memFill(cmdbuffer, sizeof(cmdbuffer), 0);
}

void KeyboardHandler(isr_registers_t* regs)
{
	uint8_t scancode = inb(0x60);
	
	//Enter Pressed
	if (scancode == 0x1C)
	{
		Scroll();
		ClearLine(promptLine);
		Scroll();
		memCopyRange(keybuffer, cmdbuffer, sizeof(keybuffer));
		memFill(keybuffer, sizeof(keybuffer), 0);
		keybuffer_ptr = 0;
		CommandParser();
		OutputAt(prompt, 0, promptLine);
		SetCursor(sizeof(prompt) - 1, promptLine);
		return;
	}
	
	//Left or Right Shift Pressed
	if (scancode == 0x2A || scancode == 0x36)
	{
		shift = true;
	}
	//Left or Right Shift Released
	else if (scancode == 0xAA || scancode == 0xB6)
	{
		shift = false;
	}
	//Left or Right Ctrl Pressed
	else if (scancode == 0x1D)
	{
		ctrl = true;
	}
	//Left or Right Ctrl Released
	else if (scancode == 0x9D)
	{
		ctrl = false;
	}
	//Left or Right Alt Pressed
	else if (scancode == 0x38)
	{
		alt = true;
	}
	//Left or Right Alt Released
	else if (scancode == 0xB8)
	{
		alt = false;
	}
	else if (keycode[scancode] != 0) //If it's in the scancode->keycode table.
	{
		if (!(scancode & 0x80)) //If it's not a release scancode.
		{
			if (keycode[scancode] == '\b')
			{
				if (keybuffer_ptr > 0) keybuffer_ptr--;
				keybuffer[keybuffer_ptr] = ' ';
			}
			else	if (keybuffer_ptr < sizeof(keybuffer))
			{
				if (shift == true) keybuffer[keybuffer_ptr] = keycode_shift[scancode];
				else keybuffer[keybuffer_ptr] = keycode[scancode];
				keybuffer_ptr++;
			}
		}
		OutputAt(prompt, 0, promptLine);
		OutputAt(keybuffer, sizeof(prompt) - 1, promptLine);
		SetCursor(sizeof(prompt) - 1 + keybuffer_ptr, promptLine);
	}
	//else it's an unhandled scancode.
}

bool StringCompare(char* buffer1, char* buffer2)
{
	while (*buffer2)
	{
		if (!(*buffer1)) return false;
		if (*buffer1++ != *buffer2++) return false;
	}
	return true;
}

bool StartsWith(char* string, char* substring)
{
	while (*substring)
	{
		if (*string++ != *substring++) return false;
	}
	return true;
}

void memCopyRange(char *source, char *dest, int length)
{
	int limit = *dest + length;
	while (*source)
	{
		if (*dest + 1 >= limit)
		{
			return;
		}
		*dest++ = *source++;
	}
}

void memFillLength(unsigned char *dest, int length, unsigned char val)
{
	int limit = *dest + length;
	while (*dest + 1 < limit)
	{
		*dest++ = val;
	}
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

void Scroll()
{
	for (int y = 1; y <= promptLine; y++)
	{
		for (int x = 0; x <  ScreenColumns; x++)
		{
			vga_memory[ScreenColumns * (y - 1) + x] = vga_memory[ScreenColumns * y + x];
		}
	}
	CursorX = 0; //Line Feed
}

void Output(char *source)
{
	while (*source)
	{
		if (CursorX == ScreenColumns - 1)
		{
			Scroll();
		}
		if (*(char *)source == '\n')
		{
			Scroll();
			source++;
			continue;
		}
		char *destination = (char *)GetVideoAddress(CursorX++, CursorY);
		*destination = *source++;
		*++destination = 0x07;
	}
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
	intToChars(byte, Chars);
	Chars[2] = 0x0; //terminator
	OutputAt(Chars, x, y);
}

void OutputHexByte(uint8_t byte)
{
	char Chars[3];
	intToChars(byte, Chars);
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

char NibbleToChar(int nibble)
{
	return (nibble < 10 ? '0' : 'A' - 10) + nibble;
}

void streamToHex(void* stream, void* hex, size_t limit)
{
	uint8_t* str = stream;
	uint8_t* out = hex;
	int i = 0;
	while (*str)
	{
		if (i+2 > limit) break;
		//if (i+5 > limit) break;
		if (*str == 0x0) break;
		char result[2] = {0};
		intToChars(*str++, result);
		//hex[i++] = '0'; //hex prefix
		//hex[i++] = 'x';
		out[i++] = result[0]; //actual hex
		out[i++] = result[1];
		//if (*++stream)
		//{
		//	hex[i++] = ','; //separator for visibility
		//	hex[i++] = ' ';
		//}
	}
	out[i] = 0x0; //terminator
}

//These functions could be useful in situations where speed is very important.
//char* Out should point to something that can hold two or more chars.
/*void intToChars(uint8_t i, char* out)
{
	out[0] = nybble_chars[i / 16];
	out[1] = nybble_chars[i % 16];
}

void intToChars16(uint16_t i, char* out)
{
	out[0] = nybble_chars[i / 16 / 16];
	out[1] = nybble_chars[i / 16 % 16];
	out[2] = nybble_chars[i % 16 / 16];
	out[3] = nybble_chars[i % 16 % 16];
}*/

void intToChars(unsigned int val, char* out)
{
	size_t p = 0;
	//out[p++] = '0';
	//out[p++] = 'x';
	size_t digits = 1;
	for (unsigned int length = val; length >= 16; length /= 16) digits++;
	for (size_t i = digits; i != 0; i--)
	{
		size_t pos = i - 1;
		out[p + pos] = nybble_chars[val % 16];
		val /= 16;
	}
	p += digits;
	//out[p] = 0x0;
}

uint32_t charsToInt(char* in)
{
	uint32_t total = 0;
	while (*in)
	{
		total = (total * 16) + char_nybbles[*(uint8_t * )in++];
	}
	return total;
}

void memPut(void* address, uint8_t value)
{
	*(uint8_t *)address = value;
}

void memPutW(void* address, uint16_t value)
{
	*(uint16_t *)address = value;
}

void memPutInt(void* address, int value)
{
	*(int *)address = value;
}

uint8_t memGet(void* address)
{
	return *(uint8_t *)address;
}

uint16_t memGetW(void* address)
{
	return *(uint16_t *)address;
}

int memGetInt(void* address)
{
	return *(int *)address;
}

void memCopy(void* srcAddress, void* destAddress)
{
	memPut(destAddress, memGet(srcAddress));
}

void memCopyInt(void* srcAddress, void* destAddress)
{
	memPutInt(destAddress, memGetInt(srcAddress));
}

void SetCursor(int x, int y)
{
	unsigned short pos = (y * ScreenColumns) + x;
	outb(VGAControlBase, 0x0F);
	outb(VGAControlBase + 1, (unsigned char)(pos & 0xFF));
	outb(VGAControlBase, 0x0E);
	outb(VGAControlBase + 1, (unsigned char)((pos >> 8) & 0xFF));
}

void memFill(void* address, int length, uint8_t value)
{
	int limit = (int)address + length;
	for (uint8_t* addr = address; (int)addr <= limit; addr++)
	{
		memPut(addr, value);
	}
}

void memFillW(void* address, int length, uint16_t value)
{
	int limit = (int)address + length;
	for (uint16_t* addr = address; (int)addr <= limit; addr+=2)
	{
		memPutW(addr, value);
	}
}

void registerISR(uint8_t idx, isr_t isr)
{
	intHandlers[idx] = isr;
}

inline void outb(uint16_t port, uint8_t val)
{
	__asm__ volatile ("outb %1, %0" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	__asm__ volatile ("inb %0, %1" : "=a"(ret) : "Nd"(port));
	return ret;
}
