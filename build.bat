@echo off
rem Build Script (build.bat)
rem Part of the ykOS Project
rem Written by J. "Yushatak" S.
rem Copyright Yushatak 2014
rem All Rights Reserved
rem
echo Building ykOS
echo -------------
path %path%;.\toolchain\bin\;.\toolchain\yk\;.\toolchain\yk\vfd;
echo =Cleaning Up...
del out\* /f /s /q
del obj\* /f /s /q
echo =Creating Directories...
mkdir obj
mkdir out
echo =Placing Empty Floppy Image...
copy build\skeleton.img out\ykOS.img /y
echo =Assembling .ASM Files...
nasm source\asm\isr.asm -f ELF -o obj\isr.o 
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\gdt.asm -f ELF -o obj\gdt.o
nasm source\asm\a20.asm -f ELF -o obj\a20.o
nasm source\asm\ctxt.asm -f ELF -o obj\ctxt.o
echo =Compiling Kernel...
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/kernel.c -o obj/kernel.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -m32 -c source/c/idt.c -o obj/idt.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/commands.c -o obj/commands.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/memory.c -o obj/memory.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/vmm.c -o obj/vmm.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/pmm.c -o obj/pmm.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -Wno-unused-variable -pedantic -std=c99 -masm=intel -c source/c/kthread.c -o obj/kthread.o
echo =Linking Kernel...
i486-elf-ld --relax -static -n -T build/kernel.ld --oformat=elf32-i386
echo =Finalizing Kernel...
i486-elf-objcopy obj/kernel.elf --only-keep-debug out/kernel.sym
i486-elf-objcopy obj/kernel.elf -S out/kernel.elf -R .eh_frame -R .comment
echo =Building Floppy Image...
vfd install
vfd start
vfd open 1: out\ykOS.img
vfd ulink 1
vfd link 1 b /L
copy out\kernel.elf b:\kernel.elf /y
vfd ulink b
vfd close 1:
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.