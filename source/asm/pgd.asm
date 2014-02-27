;Page Directory (pgd.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
global EnablePaging

align 4096
PGD:
dd PGT + 0x3
%rep 1023
dd 0
%endrep

align 4096
PGT: 
dd 0b11000000000000000000000000000000 ;kernel page
%rep 1023 
dd 0
%endrep

EnablePaging:
mov eax,PGD
mov cr3,eax
mov eax,cr0
or eax,0x80000000
mov cr0,eax
ret