OUTPUT_FORMAT("elf64-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_boot)

SECTIONS
{
	. = 0xffffffff80100000;

	.text : {
		*(.text)
	}

	.data : {
		*(.data)
	}
}
