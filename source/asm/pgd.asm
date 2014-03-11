;Page Directory (pgd.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
global EnablePaging, PGD
;Directory
align 4096
PGD:
dd PGT + 0x3 ;Entry Pointing To Table
%rep 1022
dd 0x0 ;Filler
%endrep
dd PGD + 0x3; Recursive Entry
;Table
align 4096
PGT: 
%assign i 0
%rep 1024
dd i*0x1000 + 0x3 ;Kernel Page, Identity Mapped Sequential Pages
%assign i i+1
%endrep

EnablePaging:
mov eax,PGD
mov cr3,eax
mov eax,cr0
or eax,0x80000000
mov cr0,eax
ret