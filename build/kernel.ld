STARTUP(obj/kstub.o)
/*ASM*/
INPUT(obj/pgd.o)
INPUT(obj/a20.o)
INPUT(obj/gdt.o)
INPUT(obj/isr.o)
/*C*/
INPUT(obj/kernel.o)
INPUT(obj/idt.o)
INPUT(obj/commands.o)
INPUT(obj/memory.o)
SECTIONS
{
	. = 0x100000;
	.text : {
		*(.multiboot)
		*(.text)
	}
	.rodata : {
		*(.rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(COMMON)
		*(.bss)
		*(.stack)
	}
}
OUTPUT(obj/kernel.elf)