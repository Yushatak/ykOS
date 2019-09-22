/*
Keyboard Header (keyboard.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

This provides translation for keycodes, scancodes, chars, and so on. 
It also includes variables related to keyboard handling.
*/
#include <stdbool.h>

bool shift;
bool ctrl;
bool alt;

//Buffers
char keybuffer[256];
int keybuffer_ptr;
char cmdbuffer[256];

static const char keycode[128] =
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

static const char keycode_shift[128] =
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

static const char dec_chars[] = "0123456789";
static const char nybble_chars[] = "0123456789ABCDEF";
static const uint8_t char_nybbles[] = 
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
void initialize_keyboard();