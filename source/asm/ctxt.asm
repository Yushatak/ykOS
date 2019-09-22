;Context Switch (ctxt.asm)
;Part of the ykOS Project
;Written by E. "Yushatak" S.
;Copyright Yushatak 2014-2019
;All Rights Reserved
global ctxt_sw
extern jump
;ctxt_sw(void **oldStack, void *newStack)
[BITS 32]
ctxt_sw:
pushad
mov eax, [esp+36d]
mov [eax], esp
mov eax, [esp+40d]
mov esp, eax
popad
ret