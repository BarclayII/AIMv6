OUTPUT_FORMAT("elf64-tradlittlemips")
OUTPUT_ARCH(mips)
ENTRY(_start)

SECTIONS
{
	. = 0xffffffff80300000;

	.text : {
        	*(.text)
		*(.rodata)
        }

	PROVIDE(__etext__ = . );

	.reginfo : { *(.reginfo) }

	. = ALIGN(0x1000);

	.stabs : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)
	}

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

