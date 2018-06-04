@echo off
rem Build Script (build.bat)

rem Part of the ykOS Project
rem Written by Johnathan "Yushatak" Schur.

rem Copyright Yushatak 2014

rem All Rights Reserved
rem
echo Building ykOS
echo -------------
path %path%;.\toolchain\bin\;.\toolchain\yk\;
echo =Cleaning Up...
del out\* /f /s /q
del obj\* /f /s /q
echo =Creating Directories...
mkdir obj
mkdir out
echo =Assembling .ASM Files...
nasm source\asm\boot.asm -f BIN -o out\boot.bin
nasm source\asm\isr.asm -f ELF -o obj\isr.o 
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\gdt.asm -f ELF -o obj\gdt.o
nasm source\asm\a20.asm -f ELF -o obj\a20.o
nasm source\asm\ctxt.asm -f ELF -o obj\ctxt.o
nasm source\asm\end.asm -f ELF -o obj\end.o
echo =Compiling Kernel...
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/kernel.c -o obj/kernel.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -m32 -c source/c/idt.c -o obj/idt.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/commands.c -o obj/commands.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/paging.c -o obj/paging.o -g
rem i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/vmm.c -o obj/vmm.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/pmm.c -o obj/pmm.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/memory.c -o obj/memory.o -g
rem i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/kthread.c -o obj/kthread.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/scheduler.c -o obj/scheduler.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/keyboard.c -o obj/keyboard.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/screen.c -o obj/screen.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/inline.c -o obj/inline.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/drivers/ramdisk.c -o obj/ramdisk.o -g
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/drivers/ykfs.c -o obj/ykfs.o -g
echo =Linking Kernel...
i486-elf-ld --relax -static -n obj/*.o -o out/kernel.bin --oformat=binary
echo =Building OS...
copy /b out\boot.bin+out\kernel.bin out\ykOS.bin
dd if=/dev/zero of=out\ykOS.img bs=1024 count=1440 conv=notrunc
dd if=out\ykOS.bin of=out\ykOS.img seek=0 conv=notrunc
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.