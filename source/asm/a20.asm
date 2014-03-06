[BITS 16]
global A20
A20:
in al,0x92
or al,2
out 0x92,al ;Enable A20 (PS/2+)