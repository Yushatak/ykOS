STARTUP(obj/kstub.o)
INPUT(obj/kernel.o)
INPUT(obj/isr.o)
INPUT(obj/idt.o)
INPUT(obj/commands.o)
SECTIONS
{
	. = 0x2000;
	.text : {
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
OUTPUT(obj\kernel.elf)