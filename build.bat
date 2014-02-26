@echo off
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
copy vflopBLANK.img out\vflop.img /y
echo Assembling .ASM Files...
nasm boot.asm -o out\boot
nasm isr.asm -f ELF -o obj\isr.o
echo =Compiling Kernel...
i486-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -nostdlib -masm=intel -c kernel.c -o obj/kernel.o
i486-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -nostdlib -masm=intel -m32 -c idt.c -o obj/idt.o
i486-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -nostdlib -masm=intel -c commands.c -o obj/commands.o
echo =Linking Kernel...
i486-elf-ld obj/kernel.o obj/isr.o obj/idt.o obj/commands.o --relax -static -n -T kernel.ld -o obj/kernel.elf
echo =Finalizing Kernel...
i486-elf-objcopy obj/kernel.elf --only-keep-debug out/kernel.sym
i486-elf-objcopy obj/kernel.elf --set-start 0x2000 -O binary out/kernel
echo =Building Floppy Image...
dd if=out\boot bs=512 of=out\vflop.img
dd if=out\kernel bs=512 of=out\vflop.img seek=1
echo ---------------------------------------------------------
echo Done! You may find the results in the "out" subdirectory.