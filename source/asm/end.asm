;Kernel End (end.asm)
;Part of the ykOS Project
;Written by E. "Yushatak" S.
;Copyright Yushatak 2014-2019
;All Rights Reserved
;This marks the end of the kernel, unused at this time (9/22/19).
[BITS 32]
global kernel_end
section .end
align 4096
kernel_end:
dw 0xDEAD