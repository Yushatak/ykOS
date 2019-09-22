;A20 Line Activator (a20.asm)
;Part of the ykOS Project
;Written by E. "Yushatak" S.
;Copyright Yushatak 2014-2019
;All Rights Reserved
[BITS 16]
global A20
A20:
in al,0x92
or al,2
out 0x92,al ;Enable A20 (PS/2+)