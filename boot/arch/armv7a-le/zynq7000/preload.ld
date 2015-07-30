ENTRY(bootasm)

SECTIONS
{
	. = 0x20000;
	.text : {
		*vector.o(.text)
		*(.text .rodata)
	}
	.data : {
		*(.data)
	}
}
