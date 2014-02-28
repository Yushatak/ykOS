; Bootstrap
; assemble with nasm.
; nasm -f bin -obootsect bootsect.asm

BITS 16
ORG 0x7C00

START:      JMP SHORT BEGIN
            NOP                        ; not really needed, old school style
;  +-------------------------------------------------------------+
;  | Data section of boot.asm bootstrap file                     |
;  +-------------------------------------------------------------+
bsOEM       DB 'SEKIOS12'              ; OEM String
bsSectSize  DW 512                      ; Bytes per sector
bsClustSize DB 1                        ; Sectors per cluster
bsRessect   DW 1                        ; # of reserved sectors
bsFatCnt    DB 2                        ; # of fat copies
bsRootSize  DW 224                      ; size of root directory
bsTotalSect DW 2880                    ; total # of sectors if < 32 meg
bsMedia     DB 0xF0                    ; Media Descriptor
bsFatSize   DW 9                        ; Size of each FAT
bsTrackSect DW 18                      ; Sectors per track
bsHeadCnt   DW 2                        ; number of read-write heads
bsHidenSect DD 0                        ; number of hidden sectors
bsHugeSect  DD 0                        ; if bsTotalSect is 0 this value is
                                        ; the number of sectors
bsBootDrv   DB 0                        ; holds drive that the bs came from
bsReserv    DB 0                        ; not used for anything
bsBootSign  DB 29h                      ; boot signature 29h
bsVolID     DD 0                        ; Disk volume ID also used for temp
                                        ; sector # / # sectors to load
bsVoLabel   DB 'DeviatorOS '            ; Volume Label
bsFSType    DB 'FAT12   '              ; File System type

;  +-------------------------------------------------------------+
;  | Code section of boot.asm bootstrap file                     |
;  +-------------------------------------------------------------+
BEGIN:      CLI                        ; disable interrupts
            MOV [bsBootDrv],DL          ; save drive number
            MOV AX,0x9000              ; put stack at 0x98000
            MOV SS,AX
            MOV SP,0x8000

            MOV CX,[bsTrackSect]        ; update int 1E FDC param table
            MOV BX,0x0078
            LDS SI,[DS:BX]
            MOV BYTE [SI+4], CL
            MOV BYTE [SI+9], 0x0F
            MOV AX,CS
            MOV DS,AX                  ; I know its not needed, but some
                                        ; BIOS fail if DS not set the hard
                                        ; way, my Athlon does anyways =(
            STI                        ; enable interrupts again
            PUSH DS                     
            MOV DL,[bsBootDrv]          ; DL = drive we booted from
            XOR AX,AX
            INT 0x13                    ; reset disk drive controller
            POP DS
            JC bootfail2                ; error resetting controller?
            JMP _l1
bootfail2:  JMP bootfail
_l1:
            MOV AX,0x0000
            MOV ES,AX                  ; correct segment registers
            MOV DS,AX

            MOV SI,bootmsg              ; display load message
            CALL putstr

            ; find the root directory on volume
            XOR AX,AX
            MOV AL,[bsFatCnt]          ; number of FAT mirrors on disk
            MOV BX,[bsFatSize]          ; how many sectors a FAT uses
            MUL BX                      ; AX returns FatCnt*FatSize
            ADD AX,WORD [bsHidenSect]  ; AX=AX+HiddenSectors
            ADC AX,WORD [bsHidenSect+2]; HidenSect is a double word, so...
            ADD AX,WORD [bsRessect]    ; AX now holds the sector where the
                                        ; VTOC (Volume Table Of Contents)
                                        ; starts
            MOV WORD [BootSig],AX      ; BootSig is used as temp to hold AX

            CALL checkroot              ; look into VTOC in root directory
                                        ; and see if we find our kernel image

            XOR AX,AX
            ADD AX,WORD [START]        ; [START] = number of sectors in root
            ADD AX,WORD [bsVolID]      ; sector number where kernel image
                                        ; starts, but not correct value yet..
            ADD AX,WORD [BootSig]      ; 
            SUB AX,2                    ; correction for a mis-calc
            MOV CX,WORD [bsVolID+2]    ; number of sectors

            MOV BX,0x8000
            MOV ES,BX                  ; get ready to read sectors into mem


nextsector: PUSH AX                    ; save registers
            PUSH CX
            PUSH DX
            PUSH ES

            XOR BX,BX                  ; set zero offset
            CALL readsect              ; read a sector

            POP ES                      ; restore registers
            POP DX
            POP CX
            POP AX
            MOV BX,ES
            ADD BX,0x20                ; increment address 512 bytes
            MOV ES,BX
            INC AX                      ; read next sector
            LOOPNZ nextsector
            MOV DX,0x3F2
            XOR AL,AL
            OUT DX,AL                  ; turn off floppy disk motor
            MOV SI,bootgood
            CALL putstr
            MOV DL,[bsBootDrv]          ; don't forget our bootdrive
            MOV AX,0x8000              ; set segment registers and jump
            MOV ES,AX
            MOV DS,AX
            PUSH AX
            XOR AX,AX
            PUSH AX
            RETF

checkroot:
            PUSH AX                    ; save registers
            PUSH BX
            PUSH CX
            PUSH DX
            PUSH SI
            PUSH DI

            MOV AX,0x8000              ; put root directory at 0x80000
            MOV ES,AX
            MOV AX,32                  ; AX = ((32*RootSize)/512) + 2
            MUL WORD [bsRootSize]
            DIV WORD [bsSectSize]
            MOV CX,AX                  ; cx holds # of sectors in root
            MOV WORD [START],AX
            MOV AX,WORD [BootSig]      ; get prev. saved loc. for root dir

r1:         XOR BX,BX
            PUSH CX                    ; save count
            PUSH AX                    ; save sector number
            PUSH ES
            PUSH DX
            CALL readsect
            XOR BX,BX
l_1:        MOV DI,BX                  ; set address to check from
            MOV CX,11                  ; check 11 bytes
            MOV SI,FileName            ; address of string to check with
            REPZ CMPSB
            JE foundit                  ; we found our FileName in the VTOC
            ADD BX,32                  ; check next entry
            CMP BX,[bsSectSize]        ; end of sector?
            JE l_2
            JMP l_1
l_2:        POP DX                      ; restore registers
            POP ES
            POP AX
            POP CX
            INC AX                      ; read next sector
            LOOPNZ r1
            JMP bootfail
foundit:    POP DX                      ; get these off the stack
            POP ES
            POP AX
            POP CX

            MOV DI,0x1A                ; get clustor #
            ADD DI,BX
            PUSH BX                    ; save bx for finding # of sectors
            MOV AX,[ES:DI]
            XOR BX,BX                  ; calculate sector #
            MOV BL,[bsClustSize]
            MUL BX                      ; ax holds sector #
            MOV WORD [bsVolID],AX

            POP BX                      ; get location of directory entry
            MOV DI,0x1C
            ADD DI,BX
            MOV AX,[ES:DI]              ; put number of bytes in ax
            XOR DX,DX
            MOV BX,[bsClustSize]        ; # of bytes / 512
            DIV BX
            INC AX
            MOV WORD [bsVolID+2],AX    ; save number of sectors to load

            POP DI                      ; restore registers
            POP SI
            POP DX
            POP CX
            POP BX
            POP AX

            RET                        ; return to caller

;  +-------------------------------------------------------------+
;  | Routines used over and over in boot.asm below...            |
;  +-------------------------------------------------------------+
putstr:    ; SI = address of string to display
            LODSB
            OR AL,AL
            JZ SHORT putstrd
            MOV AH,0x0E
            MOV BX,0x0007
            INT 0x10                    ; print character to CRT
            JMP putstr
putstrd:    RETN                        ; return to caller

bootfail:  ; display failure message
            MOV SI,bootbad              ; display error message
            CALL putstr
            XOR AX,AX                  ; wait for keypress
            INT 0x16
            JMP 0xFFFF:0000            ; reboot machine

readsect:  ; ES:BX = Location; AX = Sector
            MOV SI,[bsTrackSect]
            DIV SI                      ; divide logical sect by track size
            INC DL                      ; sector # begins at 1
            MOV [bsReserv],DL          ; sector to read
            XOR DX,DX                  ; logical track left in ax
            DIV WORD [bsHeadCnt]        ; leaves head in dl, cyl in ax
            MOV DH, [bsBootDrv]        ;
            XCHG DL,DH                  ; head to dh, drive to dl
            MOV CX,AX                  ; cyl to cx
            XCHG CL,CH                  ; low 8 bits of cyl to ch, hi 2 bits
            SHL CL,6                    ; shifted to bits 6 and 7
            OR CL, BYTE [bsReserv]      ; or with sector number
            MOV AL,1                    ; number of sectors
            MOV AH,2                    ; use read function of int 0x13
            PUSH DS
            INT 0x13                    ; read sector
            POP DS
            JC bootfail                ; display error message
            RET                        ; return to caller

;  +-------------------------------------------------------------+
;  | Some more data used within boot.asm bootstrap file          |
;  +-------------------------------------------------------------+
padding     TIMES 39 DB 0
bootmsg     DB 'BOOT: ',0
bootgood    DB 'GOOD',13,10,0
bootbad     DB 'BAD',13,10,0
FileName    DB 'OSLOADERCOM'
BootSig     DB 0x55, 0xAA