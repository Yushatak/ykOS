;Kernel Stub (kstub.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
;This simply calls the function marked by the "main" symbol when linked into
;the kernel to ensure proper selection of starting code.
;
extern main
kmain:
[BITS 32]
call main