;Catapult Floppy Loader (fboot.asm)
;Part of the ykOS Project
;Written by Johnathan "Yushatak" Schur
;Copyright Yushatak 2014-2018
;All Rights Reserved
;
;This is the floppy bootloader. It loads the specified number of sectors
;at 0x2000, sets up pmode and the GDT, then jumps to them.
;
[BITS 16]
[ORG 0x7C00]
jmp boot

OEM_ID db "CATAPULT"

boot:
mov ax,0x0000
cli
mov ss,ax
mov sp,0x9F00
sti
mov ds,ax
mov es,ax
mov si,Banner
call putString

mov cx,49710d ;number of sectors to read
mov bx,0x2000 ;target to read to
mov ax,0x02 ;starting position to read from
call readFloppy

mov si,Done
call putString

in al,0x92
or al,2
out 0x92,al ;Enable A20 (PS/2+)
mov eax,cr0
lgdt [cs:GDT]
or al,1
xor cx,cx
mov cr0,eax
jmp 0x08:.main32

[BITS 32]
.main32:
mov ebx,0x10 ;Data Selector
mov ds,bx
mov ss,bx 
mov es,bx
mov fs,bx
mov gs,bx
jmp 0x08:0x2000

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
int 0x10
jmp .nextChar
.return:
pop bx
pop ax
ret

;ax is absolute sector number to read
;cx is number of sectors to read
;es:bx points to buffer to read to
;auto-increments buffer and ax
readFloppy:
.begin:
test cx,cx
jz .end
dec cx
mov si,Dot
call putString
.continue:
push ax
push bx
push cx
call LBAtoCHS
mov ah,0x02
mov al,0x01 ;can do up to 128 supposedly
mov ch,byte [cyl]
mov cl,byte [sec]
mov dh,byte [hea]
mov dl,0;0 = floppy 0, 1 = floppy 2, 80 = drive 0, 81 = drive 1
int 0x13
jnc .success
xor ax,ax
int 0x13 ;reset drive
jmp .continue ;retry
.success:
pop cx
pop bx
pop ax
add bx,0x200
inc ax
jmp .begin ;keep reading sectors until done
.end:
ret

;ax = LBA in, cyl/hea/sec out, trashes dx
LBAtoCHS:
xor dx,dx
div word [spt]
inc dl
mov byte [sec],dl
xor dx,dx
div word [hc]
mov byte [hea],dl
mov byte [cyl],al
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

cyl db 0x00
hea db 0x00
sec db 0x00

hc dw 0x0002
spt dw 0x0012 ;18d

Dot db '.',0
Done db 'Kernel Loaded',13,10,0
Banner db 'Catapult Bootloader',13,10,'Copyright Yushatak 2014-2018',13,10,'All Rights Reserved',13,10,0

times 510-($-$$) db 0
STACK_END dw 0xAA55