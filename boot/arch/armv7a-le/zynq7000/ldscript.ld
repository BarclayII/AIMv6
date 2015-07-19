ENTRY(bootasm)

SECTIONS
{
	. = 0x0;
	.text : {
		//*(bootasm.o .text)
		//*(bootasm.o .rodata)
		*(.text)
		*(.rodata)
	}
}
