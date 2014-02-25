#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

struct idt_entry_struct
{
	uint16_t base_lo;
	uint16_t selector;
	uint8_t unused;
	uint8_t flags;
	uint16_t base_hi;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

typedef struct isr_registers
{
	uint32_t ds, es, fs, gs;
	uint32_t edi, esi, ebp, useless_esp, ebx, edx, ecx, eax;
	uint32_t intvec, ec;
	uint32_t eip, cs, eflags, esp, ss;
} isr_registers_t;

typedef void (*isr_t)(isr_registers_t*);
void registerISR(uint8_t idx, isr_t ISR);

static unsigned char keycode[128] =
{
		0, //unused 
		0, //esc
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
		'-', '=', '\b', '\t', 
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 
		'\n',
		0, //lctrl 
		'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'',
		'`', 
		0, //lshift
		'\\', 
		'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 
		0, //rshift
		'*',	
		0, //lalt
		' ',
		0, //caps
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //f1->f10
		0, //num lock
		0, //scroll lock
		0, //home or kp7
		0, //up or kp8
		0, //pgup or kp9
		'-',
		0, //left or kp4
		0, //kp5
		0, //right or kp6
		'+',
		0, //end or kp1
		0, //down or kp2
		0, //pgdn or kp3
		0, //insert or kp0
		0, //delete or kp .
		0, 0, 0, //unused
		0, //f11
		0, //f12
		0, 0, 0, 0, //unused
		//the rest are release scancodes mirroring the above for the most part
};

static unsigned char keycode_shift[128] =
{
		0, //unused 
		0, //esc
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
		'_', '+', '\b', '\t', 
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 
		'\n',
		0, //lctrl 
		'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"',
		'~', 
		0, //lshift
		'|', 
		'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 
		0, //rshift
		'*',	
		0, //lalt
		' ',
		0, //caps
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //f1->f10
		0, //num lock
		0, //scroll lock
		0, //home or kp7
		0, //up or kp8
		0, //pgup or kp9
		'-',
		0, //left or kp4
		0, //kp5
		0, //right or kp6
		'+',
		0, //end or kp1
		0, //down or kp2
		0, //pgdn or kp3
		0, //insert or kp0
		0, //delete or kp .
		0, 0, 0, //unused
		0, //f11
		0, //f12
		0, 0, 0, 0, //unused
		//the rest are release scancodes mirroring the above for the most part
};

static const char nybble_chars[] = "0123456789ABCDEF";
static const char char_nybbles[] = 
{
	['0'] = 0x0,
	['1'] = 0x1,
	['2'] = 0x2,
	['3'] = 0x3,
	['4'] = 0x4,
	['5'] = 0x5,
	['6'] = 0x6,
	['7'] = 0x7,
	['8'] = 0x8,
	['9'] = 0x9,
	['A'] = 0xA,
	['B'] = 0xB,
	['C'] = 0xC,
	['D'] = 0xD,
	['E'] = 0xE,
	['F'] = 0xF,
	
	['a'] = 0xA,
	['b'] = 0xB,
	['c'] = 0xC,
	['d'] = 0xD,
	['e'] = 0xE,
	['f'] = 0xF,
};
static const int keybuffer_size = 256;

//Function Declarations
uint32_t charsToInt(char* in);
void Scroll();
bool StringCompare(char* buffer1, char* buffer2);
bool StartsWith(char* string, char* substring);
void OutputToPort(char *source, int port);
void OutputLengthToPort(char *source, int port, int length);
void Interrupt(isr_registers_t* regptr);
void ClearLine(int y);
void memCopyRange(char *source, char *dest, int length);
void Output(char *source);
void OutputAt(char *source, int x, int y);
void OutputCharAt(char c, int x, int y);
void* GetVideoAddress(int x, int y);
void OutputHexByte(uint8_t byte);
void OutputHexByteAt(uint8_t byte, int x, int y);
void ClearScreen();
char NibbleToChar(int nibble);
void intToChars(unsigned int val, char* out);
void memPut(void* address, uint8_t value);
void memPutW(void* address, uint16_t value);
uint8_t memGet(void* address);
uint16_t memGetW(void* address);
void memCopy(void* srcAddress, void* destAddress);
void memPutInt(void* address, int value);
int memGetInt(void* address);
void memCopyInt(void* srcAddress, void* destAddress);
void SetCursor(int x, int y);
void memFill(void* address, int length, uint8_t value);
void memFillW(void* address, int length, uint16_t value);
void streamToHex(char* stream, char* hex, size_t limit);

//Inline ASM
static inline void outb(uint16_t port, uint8_t val);
static inline uint8_t inb(uint16_t port);
static inline void io_wait(void);

//Handlers
void KeyboardHandler(isr_registers_t* regs);