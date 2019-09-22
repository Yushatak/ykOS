;Multiboot Header multiboot.asm)
;Part of the ykOS Project
;Written by E. "Yushatak" S.
;Copyright Yushatak 2014-2019
;All Rights Reserved
;This provides multiboot support for the kernel so GRUB and others can load it.

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

section .bss
times 16384 db 0
stack_start:

section .text
global kmain
kmain:
mov esp,stack_start
extern main
call main
cli
hlt
jmp $