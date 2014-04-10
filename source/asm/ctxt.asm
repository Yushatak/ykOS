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
sti
ret