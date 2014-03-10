[BITS 32]
global LinearGDT
LinearGDT:
cli
lgdt [GDT]
jmp 0x08:.flush
.flush:
mov bx,0x10
mov ds,bx
mov es,bx
mov fs,bx
mov gs,bx
;mov ss,bx
ret

GDT_ADDR: dw 0
GDT: dw GDT_END - GDT_ADDR - 1
dd GDT_ADDR
; GDT Entry Format
; dw LIMIT0-15, BASE0-15
; db BASE16-23,ACCESS,FLAGS/LIMIT16-19,BASE24-31
dw 0xFFFF,0x0000 ;0x08, Universal Execution Segment
db 0x00,0b10011010,0b11001111,0x00
dw 0xFFFF,0x0000 ;0x10, Universal Data Segment
db 0x00,0b10010010,0b11001111,0x00
GDT_END: