;Kernel Stub (kstub.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
;This provides multiboot compatibility, sets up a 16K stack for the kernel, 
;and then simply calls the function marked by the "main" symbol when 
;linked into the kernel to ensure proper selection of starting code.
;
;Constants
cAlign equ 1<<0
cMemInfo equ 1<<1
cFlags equ cAlign | cMemInfo
cMagic equ 0x1BADB002
cChecksum equ -(cMagic + cFlags)

section .multiboot
align 4
dd cMagic
dd cFlags
dd cChecksum

section .stack
global stack_start, stack_end
stack_end:
align 4
times 16384 db 0
stack_start:

section .text
global kmain
kmain:
mov esp,stack_start
push ebx ;multiboot info
extern main
call main
cli
hlt
jmp $ ;Safety Net