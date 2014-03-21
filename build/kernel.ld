STARTUP(obj/kstub.o)
/*ASM*/
INPUT(obj/a20.o)
INPUT(obj/gdt.o)
INPUT(obj/isr.o)
/*C*/
INPUT(obj/kernel.o)
INPUT(obj/idt.o)
INPUT(obj/commands.o)
INPUT(obj/memory.o)
INPUT(obj/vmm.o)
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
	. = ALIGN(4096);
	kernel_end = .;
}
OUTPUT(obj/kernel.elf)