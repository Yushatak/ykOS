[BITS 16]
global LinearGDT
LinearGDT:
lgdt [GDT]
ret

GDT_ADDR: dw 0
GDT: dw GDT_END - GDT_ADDR - 1
dd GDT_ADDR
; GDT Entry Format
; dw LIMIT0(8), BASE0(8)
; dw ACCESS(8)/BASE1(8), BASE2(8)/FLAGS(4)/LIMIT1(4)
dw 0xFFFF,0x0000 ;0x08
dw 0x9B00,0x00CF ;0x08 flags
dw 0xFFFF,0x0000 ;0x10
dw 0x9300,0x00CF ;0x10 flags
GDT_END: