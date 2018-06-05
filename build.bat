@echo off
rem Build Script (build.bat)

rem Part of the ykOS Project
rem Written by Johnathan "Yushatak" Schur.

rem Copyright Yushatak 2014-2018
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
echo =Placing Empty Floppy Image...
copy build\skeleton.img out\ykOS.img /y
echo =Assembling .ASM Files...
rem nasm source\asm\boot.asm -f BIN -o out\boot.bin
nasm source\asm\isr.asm -f ELF -o obj\isr.o 
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\gdt.asm -f ELF -o obj\gdt.o
nasm source\asm\a20.asm -f ELF -o obj\a20.o
nasm source\asm\ctxt.asm -f ELF -o obj\ctxt.o
rem nasm source\asm\end.asm -f ELF -o obj\end.o
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
echo =Linking Kernel(s)...
rem i486-elf-ld --relax -static -n obj/*.o -o out/kernel.bin --oformat=binary
i486-elf-ld --relax -static -n -T build/kernel.ld --oformat=elf32-i386
echo =Finalizing ELF Kernel...
i486-elf-objcopy obj/kernel.elf --only-keep-debug out/kernel.sym
i486-elf-objcopy obj/kernel.elf -S out/kernel.elf -R .eh_frame -R .comment
rem echo =Building Flat Binary Image...
rem copy /b out\boot.bin+out\kernel.bin out\ykOS.bin
echo =Building Floppy Image...
imdisk -a -f out\ykOS.img -m B: -o rw
dat b /w
copy out\kernel.elf b:\kernel.elf /y
imdisk -D -m B:
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.