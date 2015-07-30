ENTRY(bootasm)

SECTIONS
{
	. = 0x1000;
	.text : {
		*bootasm.o(.text .rodata)
		*(.text .rodata)
	}
	.data : {
		*(.data)
	}
	PROVIDE(bootstack = 0x10000);
}
