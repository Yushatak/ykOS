global ctxt_sw
;ctxt_sw(void **oldStack, void *newStack)
ctxt_sw:
pusha
lea eax, [esp+36]
mov eax,esp
mov eax,[esp+40]
mov esp,eax
popa
ret