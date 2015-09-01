/* CHIP-SPECIFIC BEGIN */

/* size of address section to be filtered by SCU */
define([scu_sect_size], [1M])
[/* scu_sect_size =] scu_sect_size [*/]

/* We are in NO-SCU state now, but RAM is ready. */
MEMORY
{
	RAM (rwx) :
		ORIGIN = scu_sect_size,
		LENGTH = ram_size - scu_sect_size
}

/* CHIP-SPECIFIC END */
