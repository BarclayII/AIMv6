OUTPUT_FORMAT("elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_boot)

SECTIONS
{
	. = 0x80100000;

	.text : {
		*(.text)
	}

	.data : {
		*(.data)
	}
}
