global ctxt_sw
;ctxt_sw(void **oldStack, void *newStack)
[BITS 32]
ctxt_sw:
cli
lea eax, [esp+36]
mov eax,esp
mov eax,[esp+40]
mov esp,eax
popad
sti
ret