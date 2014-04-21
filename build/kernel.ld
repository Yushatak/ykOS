STARTUP(obj/kstub.o)
/*ASM*/
INPUT(obj/a20.o)
INPUT(obj/gdt.o)
INPUT(obj/isr.o)
INPUT(obj/ctxt.o)
/*C*/
INPUT(obj/kernel.o)
INPUT(obj/idt.o)
INPUT(obj/commands.o)
INPUT(obj/paging.o)
/*INPUT(obj/vmm.o)*/
INPUT(obj/pmm.o)
INPUT(obj/memory.o)
INPUT(obj/scheduler.o)
INPUT(obj/keyboard.o)
INPUT(obj/screen.o)
INPUT(obj/inline.o)
INPUT(obj/ramdisk.o)
INPUT(obj/ykfs.o)
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
	}
	. = ALIGN(4096);
	kernel_end = .;
}
OUTPUT(obj/kernel.elf)