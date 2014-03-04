;Kernel Stub (kstub.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
;This simply calls the function marked by the "main" symbol when linked into
;the kernel to ensure proper selection of starting code.
;
global kstack
extern main
jmp kmain ;jump over header

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

kmain:
[BITS 32]
call main