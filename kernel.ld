ENTRY(main)
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