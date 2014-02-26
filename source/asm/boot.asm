[BITS 16]
[ORG 0x7C00]

begin:
mov ax,0x0000
mov ds,ax
mov si,Banner
call putString

mov es,ax
mov bx,0x2000

mov ch,0x00
mov cl,0x02
mov dh,0x00
call readFloppy
%rep 10;+1 Sectors
call readNext
%endrep

cli
mov eax,cr0
lgdt [cs:GDT]
or al,1
xor cx,cx
mov cr0,eax
jmp 0x08:.main32

[BITS 32]
.main32:
mov bx,0x10 ;Data Selector
mov ds,bx
mov ss,bx 
mov es,bx
mov fs,bx
mov gs,bx
mov esp,0x20000 ;Kernel Stack
jmp 0x2000

[BITS 16]
putString:
push ax
push bx
mov ah,0x0E
mov bh,0x00
mov bl,0x07
.nextChar:
lodsb
or al,al
jz .return
int 10h
jmp .nextChar
.return:
pop bx
pop ax
ret

readNext:
add bx,0x200
inc cl
call readFloppy
ret

;ch track# cl sec# dh head# - es:bx points to buffer to read to
readFloppy:
mov ah,0x02
mov al,0x01
mov dl,0x00
int 13h
ret

align 4
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

Banner db 'Catapult Bootloader',13,10,'Copyright Yushatak 2014',13,10,'All Rights Reserved',13,10,0

times 510-($-$$) db 0
dw 0xAA55