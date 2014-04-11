/*
Kernel Core (kernel.c)
Part of the ykOS Project
Written by J. "Yushatak" S.
Copyright Yushatak 2014
All Rights Reserved

This is the bulk of the kernel.
*/

/*
Kernel Memory Map

0x00000000-0x00100000 - First 1MB (owned by kernel)
0x00100000-0x00400000 - Kernel (including 16K kernel stack, IDT, GDT, and extra space for growth)
0x00400000-0x00450000 - Kernel Thread Table
*/
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "kernel.h"
#include "multiboot.h"
#include "paging.h"
#include "idt.h"
#include "kthread.h"
#include "pmm.h"
#include "memory.h"
#include "commands.h"
#include "scheduler.h"

//Macro Functions - Be sure to use inline when appropriate.
#define CHECK_FLAG(flags, bit) ((flags) & (1 << (bit)))
#define BOCHS_BP() __asm__ volatile ("xchg bx,bx");
#define COMBINE_8_16(high, low) ((uint16_t)high << 8 | (uint16_t)low)
#define COMBINE_16_32(high, low) ((uint32_t)high << 16 | (uint32_t)low)
#define COMBINE_32_64(high, low) ((uint64_t)high << 32 | (uint64_t)low)

//String Declarations
char message[] = "32-Bit Kernel Loaded";
char prompt[] = "ykOS>";
char status[79] = {0};

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
int mem_total = 0;
int mem_low = 0;
int mem_high = 0;
unsigned long ticks = 0;
unsigned long tocks = 0;
const int VGABaseAddress = 0xB8000;
const int VGALimit = 0xBFFFF;
int BytesPerChar = 2;
const int VGAControlBase = 0x3D4;
isr_t intHandlers[256];
bool shift = false;
bool ctrl = false;
bool alt = false;
bool scroll = true;
static volatile bool wait = false;
int statusLine = 0;
int promptLine = 24;
multiboot_info_t* mbi;
//RTC
uint8_t second = 0;
uint8_t minute = 0;
uint8_t hour = 0;
uint8_t day = 0;
uint8_t month = 0;
uint8_t year = 0;
bool afternoon = false;

//Externs
extern void* kernel_end;
extern void* stack_start;
extern void* stack_end;

//Entry Point
int main(multiboot_info_t* boot_mbi)
{		
	mbi = boot_mbi;
	
	ring_init = false;
	
	//Memory Setup
	A20();
	LinearGDT();	
	EnablePaging();
	
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
	
	//Initialize Timer
	unsigned int num = 1193180/100;
	outb(0x43, 0x36);
	outb(0x40, (uint8_t)num & 0xFF);
	outb(0x40, num >> 8);
	
	//Generate IDT
	generateIDT();
	ClearScreen();
	registerISR(0x20, &TimerHandler);
	registerISR(0x21, &KeyboardHandler);
	registerISR(0x0E, &PageFaultHandler);
	registerISR(0x30, &DumpRegisters);
	
	//Memory Map
	if (CHECK_FLAG(mbi->flags, 1)) 
	{
		mem_low = mbi->mem_lower;
		mem_high = mbi->mem_upper;
		mem_total = mbi->mem_lower + mbi->mem_upper;
		if (CHECK_FLAG(mbi->flags, 6))
		{
			current_page = NULL;
			memory_map_t* mm_end = (memory_map_t*)(mbi->mmap_addr + mbi->mmap_length);
			for (memory_map_t* mm = (memory_map_t*)(mbi->mmap_addr);
				mm < mm_end;
				mm = (memory_map_t*)((unsigned int)mm + mm->size + sizeof(unsigned int)))
			{
				uint32_t base = COMBINE_16_32(mm->base_addr_high, mm->base_addr_low);
				uint32_t size = COMBINE_16_32(mm->length_high, mm->length_low);
				if (base > 0 && size > 0 && mm->type == 1)
				{
					pmm_claim((uint32_t*)base, size);
				}
			}
		}
		else
		{
			Output("\nInvalid Memory Map!");
			panic();
		}
	}
	else 
	{
		Output("\nInvalid Flags (0x%x)!", mbi->flags);
		panic();
	}	
	
	//Initialize Time
	uint8_t rb = cmos_read(0x0B);
	second = cmos_read(0x0);
	minute = cmos_read(0x2);
	hour = cmos_read(0x4);
	day = cmos_read(0x7);
	month = cmos_read(0x8);
	year = cmos_read(0x9);
	if (!(rb & 0x04)) //BCD->Binary If Relevant
	{
		second = (second & 0x0F) + ((second/16)*10);
		minute = (minute & 0x0F) + ((minute/16)*10);
		hour = ((hour & 0x0F) + (((hour & 0x70)/16)*10)) | (hour & 0x80);
		day = (day & 0x0F) + ((day/16)*10);
		month = (month & 0x0F) + ((month/16)*10);
		year = (year & 0x0F) + ((year/16)*10);
	}
	afternoon = hour > 12;
	hour = ((hour + 11) % 12) + 1;
	
	//Initialize Screen Elements
	OutputAt(status, 0, statusLine);
	OutputAt(prompt, 0, promptLine);
	SetCursor(sizeof(prompt) - 1, promptLine);
	
	//Initialize threading rings.
	initialize_rings();
	//Begin infinite loop.
	kernel_loop();
	return 1; //should never get here - EVER!
}

void kernel_loop()
{
	for (;;);
}

int GetMemoryCount()
{
	return mem_total;
}

void Interrupt(isr_registers_t* regs)
{	
	if (regs->intvec >= 0x28) outb(0xA0, 0x20); //Reset Slave PIC
	outb(0x20, 0x20); //Reset Master PIC
	
	if (intHandlers[regs->intvec] != 0)
	{
		((isr_t)intHandlers[regs->intvec])(regs);
	}
	else if (regs->intvec < 0x20 || regs->intvec > 0x30)
	{
		Output("\nUnhandled Interrupt %d (0x%x)!", regs->intvec, regs->intvec);
		Output("\nFault At EIP: 0x%x", regs->eip);
		panic();
		BOCHS_BP();
		__asm__ volatile("hlt");
	}
}

void rst_int_flg()
{
	if (current_thread->interrupt_state) sti();
	else cli();
}

void panic()
{
	Output("\nKernel Panic!\n");
	__asm__ volatile ("int 0x30");
}

void ClearLine(int y)
{
	for (int x = 0; x <  80; x++)
	{
		vga_memory[ScreenColumns * y + x] = 0x0720;
	}
}

void WaitKey()
{
	Output("Press a key...");
	wait = true; //Set wait status.
	__asm__ volatile("sti");
	while (wait)
	{
		continue;
	}
	__asm__ volatile("cli");
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
	if (StartsWith(cmdbuffer, "peek "))
	{
		if (splitPos == 0) Output("Invalid Argument(s).\n");
		else cmd_Peek(splitPos, (keybuffer_size - (int)splitPos) + keybuffer_ptr);
	}
	else if (StartsWith(cmdbuffer, "convert "))
	{
		if (splitPos == 0) Output("Invalid Argument(s).\n");
		else cmd_Convert(splitPos);
	}
	else if (StartsWith(cmdbuffer, "page "))
	{
		if (splitPos == 0) Output("Invalid Argument(s).\n");
		else cmd_Page(splitPos);
	}
	else if (StringCompare(cmdbuffer, "clear") || StringCompare(cmdbuffer, "cls"))
	{
		ClearScreen();
		OutputAt(prompt, 0, promptLine);
		SetCursor(sizeof(prompt) - 1, promptLine);
		CursorX = 0;
	}
	else if (StringCompare(cmdbuffer, "creg"))
	{
		cmd_Creg();
	}
	else if (StringCompare(cmdbuffer, "wait"))
	{
		WaitKey();
		Output("\nComplete.");
	}
	else if (StringCompare(cmdbuffer, "mem"))
	{
		Output("---------------\nPhysical Memory\n---------------");
		Output("\nLow: %dKB", mem_low);
		Output("\nHigh: %dKB", mem_high);
		Output("\nTotal: %dKB", mem_total);
		Output("\nAvailable: %dKB (%d%%, %d pages)", free_pages * 4,  (uint32_t)(free_pages*4*100.0/mem_total), free_pages);
		uint32_t stack_used = ((uint32_t)&stack_start-get_esp());
		uint32_t stack_total = ((uint32_t)&stack_start-(uint32_t)&stack_end);
		uint32_t stack_free = stack_total-stack_used;
		Output("\n------------\nKernel Stack\n------------");
		Output("\nTotal: %dB", stack_total);
		Output("\nFree: %dB (%d%%)", stack_free, (uint32_t)(stack_free*100.0/stack_total));
	}
	else if (StringCompare(cmdbuffer, "proc") || StringCompare(cmdbuffer, "tasklist"))
	{
		Output("---------------\nCurrent Thread\n---------------");
		Output("\nTicks: %d", current_thread->ticks);
		Output("\n------------\nThread Rings\n------------");
		Output("\nRing 0: %d Thread(s)", ring[0]->thread_count);
		Output("\nRing 1: %d Thread(s)", ring[1]->thread_count);
		Output("\nRing 2: %d Thread(s)", ring[2]->thread_count);
	}
	else if (StringCompare(cmdbuffer, "dump"))
	{
		panic();
	}
	else if (StringCompare(cmdbuffer, "ticks"))
	{
		Output("\nTicks: %d", ticks);
		Output("\nTocks: %d", tocks);
	}
	else if (StringCompare(cmdbuffer,"test"))
	{
		Output("\n%d", 0x100000000L);
	}
	else 
	{
		Output("Invalid Command.");
	}
	memFill(cmdbuffer, sizeof(cmdbuffer), 0);
}

void TestFunction()
{
	for (;;);//Output("\nTicks: %d", ticks); };
}

void KeyboardHandler(isr_registers_t* regs)
{
	//Need to design a system to handle interrupt handler feedback in the kernel
	//rather than handling it all in the handler itself. Can't interrupt if we're still 
	//in an interrupt handler!
	wait = false;
	
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

void PageFaultHandler(isr_registers_t* regs)
{
	uint32_t cr2 = get_cr2();
	Output("\n\nPage Fault At 0x%x (%x).\n\n", cr2, Get_PTE(cr2));
	DumpRegisters(regs);
	//TODO: Dynamically add page so that when it returns to the same spot, it
	//doesn't fault again, and remove the below halt/loop.
	Output("\n\nHalting...");
	__asm__ volatile("hlt");
	for (;;);
}

void TimerHandler(isr_registers_t* regs)
{
	ticks++;
	current_thread->ticks++;
	if (ticks % 1000 == 0)
	{
		if (++second >= 60)
		{
			second = 0;
			if (++minute >= 60)
			{
				minute = 0;
				if (++hour > 12)
				{
					hour = 1;
					afternoon = !afternoon;
				};
			}
		}
		OutputLine(statusLine, "\n%u/%u/%u - %u:%u:%u", month, day, year, hour, minute, second);
		Output(afternoon ? "PM" : "AM");
	}
	if (ticks % 10 == 0) next_thread();
}

void DumpRegisters(isr_registers_t* regs)
{
	Output("Register Structure: 0x%x", regs);
	Output("\nINT: 0x%x", regs->intvec);
	Output(" | Error Code: 0x%x", regs->ec);
	Output("\nEIP: 0x%x", regs->eip);
	Output("\nCR0: 0x%x", get_cr0());
	Output(" | CR2: 0x%x", get_cr2());
	Output(" | CR3: 0x%x", get_cr3());
	Output(" | CR4: 0x%x", get_cr4());
	Output("\nESP: 0x%x", regs->esp);
	Output(" | EBP: 0x%x", regs->ebp);
	Output(" | EDI: 0x%x", regs->edi);
	Output(" | ESI: 0x%x", regs->esi);
	Output("\nEAX: 0x%x", regs->eax);
	Output(" | EBX: 0x%x", regs->ebx);
	Output(" | ECX: 0x%x", regs->ecx);
	Output(" | EDX: 0x%x", regs->edx);
	Output("\nSS: 0x%x", regs->ss);
	Output(" | CS: 0x%x", regs->cs);
	Output(" | DS: 0x%x", regs->ds);
	Output(" | ES: 0x%x", regs->es);
	Output(" | FS: 0x%x", regs->fs);
	Output(" | GS: 0x%x", regs->gs);
	Output("\nEFLAGS: 0x%x", regs->eflags);
}

void ClearString(char* string, size_t length)
{
	for (int i = 0; i < length; i++)
	{
		string[i] = 0;
	}
}

bool StringCompare(char* buffer1, char* buffer2)
{
	while (*buffer1 == *buffer2)
	{
		//If both hit a terminator at the same time, return true.
		if (*buffer1 == 0) return true;
		buffer1++;
		buffer2++;
	}
	return false;
}

bool StartsWith(char* string, char* substring)
{
	while (*substring && *string)
	{
		if (*string++ != *substring++) return false;
	}
	return true;
}

void memCopyRange(char *source, char *dest, int length)
{
	int limit = *dest + length;
	while (*dest + 1 < limit)
	{
		*dest++ = *source++
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
	for (int y = statusLine + 2; y <= promptLine; y++)
	{
		for (int x = 0; x <  ScreenColumns; x++)
		{
			vga_memory[ScreenColumns * (y - 1) + x] = vga_memory[ScreenColumns * y + x];
		}
	}
	CursorX = 0; //Line Feed
}

void OutputLine(int line, const char *source, ...)
{
	ClearLine(line);
	int old_x = CursorX;
	int old_y = CursorY;
	CursorX = 0;
	CursorY = line;
	scroll = false;
	Output(source);
	scroll = true;
	CursorX = old_x;
	CursorY = old_y;
}

void Output(const char *source, ...)
{
	char** arg = (char**)&source;
	arg++;
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
					intToDecChars(*((int*)arg++), buffer, 32);
					ptr=buffer;
					goto string;
					break;
				case 'u':
					uintToDecChars(*((unsigned int*)arg++), buffer, 32);
					ptr = buffer;
					goto string;
					break;
				case 'h':
				case 'x':
					uintToHexChars(*((int*)arg++), buffer, 32);
					ptr=buffer;
					goto string;
					break;
				case 's':
					ptr=*arg++;
					if (!ptr)
						ptr = "NULL";
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
		uintToHexChars(*str++, result, 2);
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
/*void uintToHexChars(uint8_t i, char* out)
{
	out[0] = nybble_chars[i / 16];
	out[1] = nybble_chars[i % 16];
}

void uintToHexChars16(uint16_t i, char* out)
{
	out[0] = nybble_chars[i / 16 / 16];
	out[1] = nybble_chars[i / 16 % 16];
	out[2] = nybble_chars[i % 16 / 16];
	out[3] = nybble_chars[i % 16 % 16];
}*/

void uintToHexChars(unsigned int val, char* out, size_t len)
{
	ClearString(out, len);
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

void uintToDecChars(unsigned int val, char* out, size_t len)
{
	ClearString(out, len);
	size_t p = 0;
	size_t digits = 1;
	for (unsigned int length = val; length >= 10; length /= 10) digits++;
	for (size_t i = digits; i != 0; i--)
	{
		size_t pos = i - 1;
		out[p + pos] = dec_chars[val % 10];
		val /= 10;
	}
	p += digits;
}

void intToDecChars(int val, char* out, size_t len)
{
	ClearString(out, len);
	size_t p = 0;
	if (val < 0) //If negative, prefix it.
	{
		out[p] = '-';
		p++;
	}
	size_t digits = 1;
	for (int length = val; length >= 10; length /= 10) digits++;
	for (size_t i = digits; i != 0; i--)
	{
		size_t pos = i - 1;
		out[p + pos] = dec_chars[val % 10];
		val /= 10;
	}
	p += digits;
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

inline void halt()
{
	__asm__ volatile("hlt");
}

inline uint32_t get_eax()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, eax" : "=b"(ret));
	return ret;
}

inline uint32_t get_ebx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, ebx" : "=a"(ret));
	return ret;
}

inline uint32_t get_ecx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, eax" : "=a"(ret));
	return ret;
}

inline uint32_t get_edx()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, edx" : "=a"(ret));
	return ret;
}

inline uint32_t get_esp()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, esp" : "=a"(ret));
	return ret;
}

inline uint32_t get_ebp()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, ebp" : "=a"(ret));
	return ret;
}

inline uint32_t get_esi()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, esi" : "=a"(ret));
	return ret;
}

inline uint32_t get_edi()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, edi" : "=a"(ret));
	return ret;
}

inline uint32_t get_cr0()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr0" : "=a"(ret));
	return ret;
}

inline uint32_t get_cr2()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr2" : "=a"(ret));
	return ret;
}
	
inline uint32_t get_cr3()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr3" : "=a"(ret));
	return ret;
}
	
inline uint32_t get_cr4()
{
	uint32_t ret = 0;
	__asm__ volatile ("mov %0, cr4" : "=a"(ret));
	return ret;
}
	
/*inline uint64_t get_cr8()
{
	uint64_t ret = 0;
	__asm__ volatile ("mov %0, cr8" : "=a"(ret));
	return ret;
}*/

void cli()
{
	current_thread->interrupt_state = false;
	__asm__ volatile("cli");
}

inline void untracked_cli()
{
	__asm__ volatile("cli");
}

void sti()
{
	current_thread->interrupt_state = true;
	__asm__ volatile("sti");
}

inline void untracked_sti()
{
	__asm__ volatile("sti");
}

uint8_t cmos_read(uint8_t reg)
{
	outb(0x70, reg);
	return inb(0x71);
}