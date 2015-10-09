OUTPUT_FORMAT("elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_boot)

SECTIONS
{
	.text : {
		*(.text)
	}

	.data : {
		*(.data)
	}
}
