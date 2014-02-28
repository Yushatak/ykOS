;Catapult Loader (boot.asm)
;Part of the ykOS Project
;Written by J. "Yushatak" S.
;Copyright Yushatak 2014
;All Rights Reserved
;
;This is the floppy bootloader. It loads the specified (at the %rep line) 
;number of sectors at 0x2000, sets up pmode and the GDT, then jumps
;to them.
;
[BITS 16]
[ORG 0x7C00]
jmp boot

OEM_ID db "CATAPULT"

boot:
cli
mov ax,0xFFFF
mov ss,ax
mov sp,0xFFFF
mov ax,0x0000
mov ds,ax
mov es,ax
mov si,Banner
call putString

mov cx,11d
mov bx,0x2000
mov ax,0x02;
call readFloppy

mov si,Done
call putString

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
mov esp,0x40000 ;Kernel Stack
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
int 0x10
jmp .nextChar
.return:
pop bx
pop ax
ret

;ax is absolute sector number to read, es:bx points to buffer to read to
;auto-increments buffer and ax
readFloppy:
.begin:
dec cx
cmp cx,0
jne .continue
ret
.continue:
push ax
push bx
push cx
call LBAtoCHS
mov ah,0x02
mov al,0x01
mov ch,byte [cyl]
mov cl,byte [sec]
mov dh,byte [hea]
mov dl,0x00;drive
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
loop .begin ;keep reading sectors until done
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

Done db 'Kernel Loaded',13,10,0
Banner db 'Catapult Bootloader',13,10,'Copyright Yushatak 2014',13,10,'All Rights Reserved',13,10,0

times 510-($-$$) db 0
dw 0xAA55