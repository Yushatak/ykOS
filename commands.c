#include <stdint.h>
#include <stdbool.h>
#include "kernel.h"
#include "commands.h"

void cmd_Convert(char* args)
{
	char result[keybuffer_size*6]; //up to 6 chars out per input char, so multiply by 6
	streamToHex((unsigned char*)args, result, sizeof(result));
	Output(result);
}

void cmd_Peek(char* args, size_t argsize)
{
	Output(args);
	Output(":");
	char Chars[16] = {0};
	intToChars(*(uint32_t *)charsToInt(args), Chars);
	Output(Chars);
}