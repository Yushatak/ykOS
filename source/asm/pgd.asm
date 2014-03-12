;Page Directory (pgd.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
extern kernel_end
global enablePaging

enablePaging:
mov eax,kernel_end
mov cr3,eax
mov eax,cr0
or eax,0x80000000
mov cr0,eax
ret