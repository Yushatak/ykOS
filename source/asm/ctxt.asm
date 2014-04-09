global ctxt_sw
;ctxt_sw(void **oldStack, void *newStack)
[BITS 32]
ctxt_sw:
pushad
lea eax, [esp+36d]
mov [eax], esp
mov eax, [esp+40d]
mov esp, eax
popad
ret