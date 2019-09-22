;Catapult Loader (boot.asm)
;Part of the ykOS Project
;Written by E. "Yushatak" S.
;Copyright Yushatak 2014-2019
;All Rights Reserved
;
;This is the bootloader. It loads the specified number of sectors
;at 0x10000 then jumps to them.
;
;Memory Map
;0x0x00007000->0x00007C00 = Stack (growing downward)
;0x00000000->0x00007E00 = Bootloader (this code)
;0x00007E00->0x0000FFFF = Unused
;0x00010000->0x00010000 + Kernel Size = Kernel
;Notes
;- [es:bx] means (es*0x10)+bx
;- If the drive letter is wrong it will quite possibly freeze with one sector load 
;dot since it prints those before actually reading.
;- LBA sector numbers start at 0, CHS sector numbers start at 1.
[BITS 16]
[ORG 0x7C00] ;and this is a bootloader, so it's 512 bytes, so it ends at 7E00
begin:
jmp 0x0000:boot ;reset cs to zero
boot:
xor ax,ax ;clear AX
mov ds,ax ;set ds to zero
mov byte [disk], dl ;store disk number provided by BIOS
;turn off interrupts before setting stack/ptr, this is early 8088 bug, so we omitted since 486+
mov ax,begin
mov ss,ax ;set stack segment
mov sp,0xC00 ;stack (descending) starts before the bootloader and extends down
;turn interrupts back on here if 8088, see above
mov si,Banner
call putString

mov cx,64d ;number of sectors to read / sectors per read
mov bx,0x1000 ;this will go in segment
mov es,bx ;segment of 0x1000
mov bx,0x0000 ;target address of 0x0000 (remember this is relative to segment)
;All that ends up with reading to 0x10000
mov ax,0x01 ;starting position to read from
call readDisk

mov si,Done
call putString

jmp 0x1000:0x0000 ;far jump to 0x10000, where we loaded the kernel

[BITS 16]
putString:
push ax
push bx
mov ah,0x0E
mov bh,0x00
mov bl,0x07
.nextChar:
cld ;forward auto-increment si/di, std would be reverse
lodsb
or al,al
jz .return
int 0x10
jmp .nextChar
.return:
pop bx
pop ax
ret

;parameters are in the DAP, disk number is loaded from [disk]
readDiskExtended:
mov si,DAP
mov ah,42h
mov dl,byte [disk]
int 0x13
ret

;ax is starting sector number to read
;cx is number of sectors to read
;es:bx points to buffer to read to
;auto-increments buffer and ax
;trashes dx
readDisk:
.begin:
test cx,cx
jz .end
dec cx
pusha
mov si,Dot
call putString ;Put a dot on-screen for each batch of sectors read
.continue:
call LBAtoCHS
mov ah,0x02 ;read sector routine of int13
mov al,1d ;sectors to read at once, most is 128
mov ch,byte [cyl] ;load cylinders of CHS address into ch
mov cl,byte [sec] ;load sectors of CHS address into cl
mov dh,byte [hea] ;load heads of CHS address into dh
mov dl,byte [disk] ;load disk number provided by BIOS on boot into dl
;dl values: 0 = floppy 0, 1 = floppy 2, 80 = drive 0, 81 = drive 1
int 0x13
jnc .success ;if carry isn't set, read was fine, so skip over the reset/retry code
push ax ;store ax, we're gonna need to set it to other things
xor ax,ax ;clear ax for int13 parameter usage
int 0x13 ;reset drive
pop ax ;restore ax
jmp .continue ;retry
.success: ;read worked
popa
;When reading 128 sectors at a clip we need to increment target by 0x10000
;0x10000 doesn't fit in a word, so we have to modify the segment.
;We increment the segment register (es) by 1 instead and keep bx as 0.
;mov bx,es
;inc bx
;mov es,bx
;xor bx,bx
;Would use the below for sector-by-sector reads <= 0xFFFF sectors
add bx,0x200 ;0x200 = 512d (one sector)
inc ax ;increment sector 'cursor'
jmp .begin ;keep reading sectors until done
.end:
ret

;ax = LBA in, cyl/hea/sec out to [cyl] [hea] [sec]
LBAtoCHS:
push dx
xor dx,dx
div word [spt]
inc dl
mov byte [sec],dl
xor dx,dx
div word [hc]
mov byte [hea],dl
mov byte [cyl],al
pop dx
ret

DAP:
db 10h,0
dw 65535
dd 0x00010000
dq 1

disk db 0x00

cyl db 0x00
hea db 0x00
sec db 0x00

hc dw 0x0002
spt dw 0x0012 ;18d

Dot db '.',0
Done db 13,10,'Kernel Loaded',13,10,0
Banner db 'Catapult Bootloader',13,10,'Copyright Yushatak 2014-2019',13,10,'All Rights Reserved',13,10,0

times 510-($-$$) db 0
STACK_END dw 0xAA55