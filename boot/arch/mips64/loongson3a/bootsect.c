/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <elf.h>

#define BUFFER			0x80180000
#define ELFMAGIC		0x464c457f

#define PART0_ENTRY		0x1be
#define PART1_ENTRY		0x1ce
#define PART2_ENTRY		0x1de
#define PART3_ENTRY		0x1ee

#define PART_SECTOR_START	0x8
#define PART_SECTOR_NUM		0xc

#define SECTOR_SIZE		512

typedef void (*entry_t)();

/*
 * Loongson 3A starts up by executing PMON in the programmable ROM.
 *
 * PMON works in 32-bit, and so does MBR.
 * Therefore things like "unsigned long long" should not appear in bootloader
 * code.
 *
 * PMON originally does not support MBR bootloader code, so I modified PMON
 * code to provide some help for MBR.  I found that a bootloader doing all
 * works on its own is unrealistic for MIPS:  I turned on size optimization to
 * save as much space as possible, skipped ELF verifications, introduced three
 * helper functions as well as the handle for reading the hard disk, fixed MBR
 * to load kernel from the second partition, skipped partition type
 * verification, and still nearly ran out of 446-byte space.
 *
 * The modified PMON passes four arguments to MBR:
 * @fd:       file descriptor representing the hard disk.
 * @findsect: function to jump to the specified sector.
 * @read:     read from the file descriptor, similar to read(2)
 * @lseek:    reposition read offset, similar to lseek(2)
 *
 * The modified PMON loads MBR (this function, plus the partition entries)
 * to virtual address 0x80100000.
 */

void __attribute__((noreturn)) _boot(int fd,
    void (*findsect)(int, unsigned int),
    int (*read)(int, void *, unsigned int),
    int (*lseek)(int, unsigned int, int))
{
	int i, phnum;
	entry_t entry;

	unsigned int sector_start;
	unsigned int pos = 0;

	unsigned char *buf = (unsigned char *)BUFFER;

	unsigned char *part1 = ((unsigned char *)_boot) + PART1_ENTRY;

	/*
	 * GCC 4.4.0 (which is the version used by Loongson Tech.) had
	 * difficulty dereferencing unaligned address.
	 *
	 * In MIPS, the addresses of load/store instructions other than
	 * LWL, LWR, SWL, SWR must be *aligned*: LH/SH operates on
	 * addresses aligned to 2 bytes, LW/SW on 4 bytes, etc.
	 * Since (part1 + PART_SECTOR_START) is not aligned to 4 bytes,
	 * directly dereferencing such address throws an exception.
	 *
	 * The latest GCC version does not have this bug.
	 */

	/* Locate the starting sector of the second partition */
	sector_start = *(unsigned short *)(part1 + PART_SECTOR_START + 2);
	sector_start = (sector_start << 16) +
	    *(unsigned short *)(part1 + PART_SECTOR_START);

	findsect(fd, sector_start);

	/* Read the kernel ELF header */
	read(fd, buf, sizeof(struct elf64hdr));
	pos += sizeof(struct elf64hdr);

	struct elf64hdr *eh = (struct elf64hdr *)buf;

	/* Find the number of program headers (segments) */
	phnum = eh->e_phnum;

	/* Locate the kernel entry virtual address */
	entry = (entry_t)(unsigned int)(eh->e_entry);

	for (i = 0; i < phnum; ++i) {
		/* Load each loadable segment into memory */
		if (read(fd, buf, sizeof(struct elf64_phdr)) == -1)
			/*
			 * These breakpoints shouldn't happen and are only for
			 * diagnostics.
			 */
			asm volatile ("break 8");
		pos += sizeof(struct elf64_phdr);

		struct elf64_phdr *ph = (struct elf64_phdr *)buf;
		if (ph->p_type == PT_LOAD) {
			int off = (unsigned int)(ph->p_offset) - pos;
			if (off != 0 && lseek(fd, off, 1) == -1)
				asm volatile ("break 9");
			if (read(fd,
			    (unsigned char *)(unsigned int)(ph->p_vaddr),
			    (unsigned int)(ph->p_filesz)) == -1)
				asm volatile ("break 10");
			if (off != 0 && lseek(fd, -off, 1) == -1)
				asm volatile ("break 11");
		}
	}

	/* Jump to entry */
	(*entry)();
	for (;;)
		/* nothing */;
}
