del boot
del kernel
del kernel.o
del isr.o
del idt.o
del kernel.elf
nasm boot.asm
nasm isr.asm -f ELF -o isr.o

C:\cygwin64\home\Yushatak\opt\cross\bin\i586-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -nostdlib -c kernel.c -o kernel.o
C:\cygwin64\home\Yushatak\opt\cross\bin\i586-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -nostdlib -m32 -c idt.c -o idt.o
C:\cygwin64\home\Yushatak\opt\cross\bin\i586-elf-gcc -ffreestanding -Wall -Werror -pedantic -std=c99 -masm=intel -nostdlib -c commands.c -o commands.o

C:\cygwin64\home\Yushatak\opt\cross\bin\i586-elf-ld kernel.o isr.o idt.o commands.o --relax -static -n -T kernel.ld -o kernel.elf
objcopy kernel.elf --only-keep-debug kernel.sym
objcopy kernel.elf --set-start 0x2000 -O binary kernel
rem  --strip-all --strip-debug --strip-dwo --strip-unneeded don't do anything as of 2/25/14 for our kernel
copy vflopBLANK.img vflop.img /y

dd if=boot bs=512 of=vflop.img

dd if=kernel bs=512 of=vflop.img seek=1

copy vflop.img c:\ /y
