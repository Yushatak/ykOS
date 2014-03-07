@echo off
rem Build Script (build.bat)
rem Part of the ykOS Project
rem Written by J. "Yushatak" S.
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
rem echo =Placing Empty Floppy Image...
rem copy build\vflopBLANK.img out\vflop.img /y
echo =Assembling .ASM Files...
rem nasm source\asm\boot.asm -O0 -o out\boot
nasm source\asm\isr.asm -f ELF -o obj\isr.o
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\multiboot.asm -f ELF -o obj\multiboot.o
nasm source\asm\pgd.asm -f ELF -o obj\pgd.o
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
rem echo =Building Floppy Image...
rem dd if=out\boot bs=512 of=out\vflop.img count=1
rem dd if=out\kernel bs=512 of=out\vflop.img seek=1
rem copy out\vflop.img c:\vflop.img /y
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.