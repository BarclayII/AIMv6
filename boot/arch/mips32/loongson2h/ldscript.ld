OUTPUT_FORMAT("elf32-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_reset)

SECTIONS
{
	. = 0xffffffff88c00000;

	.text : {
        	*(.text)
		*(.rodata)
        }

	PROVIDE(__etext__ = . );

	.reginfo : { *(.reginfo) }

	. = ALIGN(0x1000);

	PROVIDE(__data__ = . );

	.data : {
		*(.data)
	}

	PROVIDE(__edata__ = . );

	.bss  : {
		*(.dynbss)
		*(.bss)
	}

	PROVIDE(__end__ = . );
}

