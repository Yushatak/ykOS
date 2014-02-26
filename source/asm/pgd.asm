;Page Directory (pgd.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
global EnablePaging

align 4096
PGD:
dd 0b11001000000000000000000000000000 ;points to PGT
%rep 1023
dd 0b11001000000000000000000000000000 ;unused page
%endrep

align 4096
PGT: 
dd 0b11001000000000000000000000000000 ;kernel page
%rep 1023 
dd 0b11001000000000000000000000000000 ;unused page
%endrep

EnablePaging:
mov eax,PGD
or eax,PGT
mov [PGD],eax
mov eax,PGD
mov cr3,eax
mov eax,cr0
or eax,0x80000001
mov cr0,eax
ret