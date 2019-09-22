/*
Screen Header (screen.h)
Part of the ykOS Project
Written by E. "Yushatak" S.
Copyright Yushatak 2014-2019
All Rights Reserved

This provides functions and variables for interaction with a 80x25 text-mode screen.
*/

int statusLine;
int promptLine;
int CursorX;
int CursorY;
int ScreenColumns;
int ScreenRows;
int BytesPerChar;
bool scroll;
uint16_t* vga_memory;

//Function Declarations
void initialize_screen();
void Output(const char *source, ...);
void OutputChar(char c);
void OutputAt(char *source, int x, int y);
void OutputCharAt(char c, int x, int y);
void* GetVideoAddress(int x, int y);
void SetCursor(int x, int y);
void OutputLine(int line, const char *source, ...);
void base_Output(const char *source, va_list args);
void OutputHexByte(uint8_t byte);
void OutputHexByteAt(uint8_t byte, int x, int y);
void ClearLine(int y);
void ClearScreen();
void Scroll();