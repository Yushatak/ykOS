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