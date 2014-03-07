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
rem nasm source\asm\boot.asm -O0 -o out\boot
nasm source\asm\isr.asm -f ELF -o obj\isr.o
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\multiboot.asm -f ELF -o obj\multiboot.o
rem nasm source\asm\pgd.asm -f ELF -o obj\pgd.o
nasm source\asm\gdt.asm -f ELF -o obj\gdt.o
nasm source\asm\a20.asm -f ELF -o obj\a20.o
echo =Compiling Kernel...
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -c source/c/kernel.c -o obj/kernel.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -m32 -c source/c/idt.c -o obj/idt.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -c source/c/commands.c -o obj/commands.o
echo =Linking Kernel...
i486-elf-ld --relax -static -n -T build/kernel.ld --oformat=elf32-i386
echo =Finalizing Kernel...
i486-elf-objcopy obj/kernel.elf --only-keep-debug out/kernel.sym
rem i486-elf-objcopy obj/kernel.elf --set-start 0x2000 -O binary out/kernel
copy obj\kernel.elf out\kernel.elf /y
echo =Building Floppy Image...
vfd install
vfd start
vfd open 1: out\ykOS.img
vfd ulink 1
vfd link 1 b /L
copy out\kernel.elf b:\kernel.elf /y
vfd ulink b
vfd close 1:
rem dd if=out\boot bs=512 of=out\vflop.img count=1
rem dd if=out\kernel bs=512 of=out\vflop.img seek=1
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.