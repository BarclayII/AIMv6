ENTRY(mbr_bootmain)

MEMORY
{
	MBR (rwx) :
		ORIGIN = 1M,
		LENGTH = 512
}

SECTIONS
{
	.text : {
		*(.text .rodata)
	}
	.data : {
		*(.data)
	}
	.bss : {
		*(.bss)
	}
}
