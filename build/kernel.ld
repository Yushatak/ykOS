STARTUP(obj/kstub.o)
INPUT(obj/kernel.o)
INPUT(obj/isr.o)
INPUT(obj/idt.o)
INPUT(obj/commands.o)
INPUT(obj/pgd.o)
INPUT(obj/a20.o)
INPUT(obj/gdt.o)
SECTIONS
{
	. = 0x2000;
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
		*(.bootstrap_stack)
	}
}
OUTPUT(obj/kernel.elf)