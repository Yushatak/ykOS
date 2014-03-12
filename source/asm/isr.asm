;Interrupt Service Routines (isr.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
;This contains the ISR stub and macros/macro calls to generate stubs to
;set up values for each, then call the main stub, which then calls the c
;stub.
;
[BITS 32]
global ISR7, ISR15 ;Spurious, no macro to make them global.
extern Interrupt

ISR:
pushad
push esp
push ss
push ds
push es
push fs
push gs
mov eax,0x10
mov ds,eax
mov es,eax
mov fs,eax
mov gs,eax
push esp
call Interrupt
pop esp
pop gs
pop fs
pop es
pop ds
pop ss
pop esp
popad
add esp,8
iret

%macro ISR_NOEC 1
global ISR%1
ISR%1:
push byte 0
push byte %1
jmp ISR
%endmacro

%macro ISR_EC 1
global ISR%1
ISR%1:
push byte %1
jmp ISR
%endmacro

%macro IRQ 2
global IRQ%2
IRQ%2:
push byte 0
push byte %1
jmp ISR
%endmacro

ISR_NOEC 0 ;0x00
ISR_NOEC 1 ;0x01
ISR_NOEC 2 ;0x02
ISR_NOEC 3 ;0x03
ISR_NOEC 4 ;0x04
ISR_NOEC 5 ;0x05
ISR_NOEC 6 ;0x06
;Spurious, handled below. ;0x07
ISR_EC 8 ;0x08
ISR_NOEC 9 ;0x09
ISR_EC 10 ;0x0A
ISR_EC 11 ;0x0B
ISR_EC 12 ;0x0C
ISR_EC 13 ;0x0D
ISR_EC 14 ;0x0E
;Spurious, handled below. ;0x0F
ISR_NOEC 16 ;0x10
ISR_NOEC 17 ;0x11
ISR_NOEC 18 ;0x12
ISR_NOEC 19 ;0x13
ISR_NOEC 20 ;0x14
ISR_NOEC 21 ;0x15
ISR_NOEC 22 ;0x16
ISR_NOEC 23 ;0x17
ISR_NOEC 24 ;0x18
ISR_NOEC 25 ;0x19
ISR_NOEC 26 ;0x1A
ISR_NOEC 27 ;0x1B
ISR_NOEC 28 ;0x1C
ISR_NOEC 29 ;0x1D
ISR_NOEC 30 ;0x1E
ISR_NOEC 31 ;0x1F
IRQ 32, 0 ;0x20
IRQ 33, 1 ;0x21
IRQ 34, 2 ;0x22
IRQ 35, 3 ;0x23
IRQ 36, 4 ;0x24
IRQ 37, 5 ;0x25
IRQ 38, 6 ;0x26
IRQ 39, 7 ;0x27
IRQ 40, 8 ;0x28
IRQ 41, 9 ;0x29
IRQ 42, 10 ;0x2A
IRQ 43, 11 ;0x2B
IRQ 44, 12 ;0x2C
IRQ 45, 13 ;0x2D
IRQ 46, 14 ;0x2E
IRQ 47, 15 ;0x2F

ISR7:
        push eax
        mov al,0x0B                     ;al = "read ISR" command
        out 0x20,al                     ;Send "read ISR" command
        in al,0xA0                      ;Dummy read (just a small delay)
        in al,0x20                      ;al = ISR for master PIC
        test al,0x80                    ;Is IRQ 7 in service?
        jne .realIRQ7                   ; yes
        pop eax                         ; no, spurious IRQ7 (do nothing)
        iret
 
.realIRQ7:
	cli
        pop eax
	push byte 0
        push 0x07
        jmp ISR
 
ISR15:
        push eax
        mov al,0x0B                     ;al = "read ISR" command
        out 0xA0,al                     ;Send "read ISR" command
        in al,0x20                      ;Dummy read (just a small delay)
        in al,0xA0                      ;al = ISR for slave PIC
        test al,0x80                    ;Is IRQ 15 in service?
        jne .realIRQ15                  ; yes
        mov al,0x20                     ; no, spurious IRQ15 (send EOI to master only)
        out 0x20,al
        pop eax    
        iret
 
.realIRQ15:
	cli
        pop eax
	push byte 0
        push 0x0F
        jmp ISR