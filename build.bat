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
echo =Placing Empty Floppy Image...
copy build\vflopBLANK.img out\vflop.img /y
echo =Assembling .ASM Files...
nasm source\asm\boot.asm -o out\boot
nasm source\asm\isr.asm -f ELF -o obj\isr.o
nasm source\asm\kstub.asm -f ELF -o obj\kstub.o
nasm source\asm\pgd.asm -f ELF -o obj\pgd.o
echo =Compiling Kernel...
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -c source/c/kernel.c -o obj/kernel.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -m32 -c source/c/idt.c -o obj/idt.o
i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -c source/c/commands.c -o obj/commands.o
rem i486-elf-gcc -Os -ffreestanding -Wall -Werror -pedantic -std=c99 -nostdlib -masm=intel -
c source/c/page.c -o obj/page.o
echo =Linking Kernel...
i486-elf-ld --relax -static -n -T build/kernel.ld
echo =Finalizing Kernel...
i486-elf-objcopy obj/kernel.elf --only-keep-debug out/kernel.sym
i486-elf-objcopy obj/kernel.elf --set-start 0x2000 -O binary out/kernel
echo =Building Floppy Image...
dd if=out\boot bs=512 of=out\vflop.img
dd if=out\kernel bs=512 of=out\vflop.img seek=1
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.