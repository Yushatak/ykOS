global ctxt_sw
extern jump
;ctxt_sw(void **oldStack, void *newStack)
[BITS 32]
ctxt_sw:
pushad
lea eax, [esp+36d]
mov [eax], esp
mov eax, [esp+40d]
mov esp, eax
popad
sti
jmp [esp] ;doesn't do EIP but does let them swap properly