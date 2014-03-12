STARTUP(obj/kstub.o)
/*ASM*/
INPUT(obj/a20.o)
INPUT(obj/gdt.o)
INPUT(obj/isr.o)
INPUT(obj/end.o)
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
	.end : {
		*(.end)
	}
}
OUTPUT(obj/kernel.elf)