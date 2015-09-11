/*
 * Copyright (C) 2015 Gan Quan <coin2028@hotmail.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <elf.h>
#include <endian.h>

#include "include/stab.h"

unsigned char typecodes[] = {
	N_GSYM, N_FNAME, N_FUN, N_STSYM, N_LCSYM, N_MAIN, N_PC, N_RSYM,
	N_SLINE, N_DSLINE, N_BSLINE, N_SSYM, N_SO, N_LSYM, N_BINCL,
	N_SOL, N_PSYM, N_EINCL, N_ENTRY, N_LBRAC, N_EXCL, N_RBRAC,
	N_BCOMM, N_ECOMM, N_ECOML, N_LENG
};

const char *types[256] = {
	[N_GSYM]	= "global symbol",
	[N_FNAME]	= "f77 func",
	[N_FUN]		= "func",
	[N_STSYM]	= "data segment var",
	[N_LCSYM]	= "bss segment var",
	[N_MAIN]	= "main func",
	[N_PC]		= "global Pascal symbol",
	[N_RSYM]	= "register var",
	[N_SLINE]	= "text line #",
	[N_DSLINE]	= "data line #",
	[N_BSLINE]	= "bss line #",
	[N_SSYM]	= "struct/union element",
	[N_SO]		= "source file",
	[N_LSYM]	= "local var",
	[N_BINCL]	= "include begin",
	[N_SOL]		= "include source file",
	[N_PSYM]	= "argument var",
	[N_EINCL]	= "include end",
	[N_ENTRY]	= "alt. entry",
	[N_LBRAC]	= "left brace",
	[N_EXCL]	= "deleted include",
	[N_RBRAC]	= "right brace",
	[N_BCOMM]	= "common begin",
	[N_ECOMM]	= "common end",
	[N_ECOML]	= "common end local",
	[N_LENG]	= "length"
};

#define NR_ELEMENTS(arr)	(sizeof(arr) / sizeof(arr[0]))

#define STABSTR		".stabstr"
#define STABS		".stabs"

unsigned int elf32, elf_little_endian;

inline uint16_t htofs(uint16_t hostbits)
{
	return elf_little_endian ? htole16(hostbits) : htobe16(hostbits);
}

inline uint32_t htofl(uint32_t hostbits)
{
	return elf_little_endian ? htole32(hostbits) : htobe32(hostbits);
}

inline uint64_t htofx(uint64_t hostbits)
{
	return elf_little_endian ? htole64(hostbits) : htobe64(hostbits);
}

inline uint16_t ftohs(uint16_t hostbits)
{
	return elf_little_endian ? le16toh(hostbits) : be16toh(hostbits);
}

inline uint32_t ftohl(uint32_t hostbits)
{
	return elf_little_endian ? le32toh(hostbits) : be32toh(hostbits);
}

inline uint64_t ftohx(uint64_t hostbits)
{
	return elf_little_endian ? le64toh(hostbits) : be64toh(hostbits);
}

void usage(char *prog)
{
	printf("Usage: %s <ELF-file>\n", prog);
	exit(1);
}

void readstab64(int fd, Elf64_Ehdr *eh64)
{
	Elf64_Shdr es64, es64_stab;
	unsigned int i;
	char *shstrtab = NULL, *stabstr = NULL;
	struct stab stab;
	off_t cur;

	if (lseek(fd, eh64->e_shoff, SEEK_SET) == -1)
		err(4, "lseek");
	/* Locate section header string table (.shstrtab) first */
	if (lseek(fd, eh64->e_shentsize * eh64->e_shstrndx, SEEK_CUR) == -1)
		err(4, "lseek");
	if (read(fd, &es64, sizeof(es64)) == -1)
		err(4, "read");
	if ((shstrtab = (char *)malloc(es64.sh_size)) == NULL)
		err(4, "malloc");
	if (lseek(fd, es64.sh_offset, SEEK_SET) == -1)
		err(4, "lseek");
	if (read(fd, shstrtab, es64.sh_size) == -1)
		err(4, "read");
	/* Relocate to the start of section header table */
	if (lseek(fd, eh64->e_shoff, SEEK_SET) == -1)
		err(4, "lseek");

	/* Load the stab string section */
	for (i = 0; i < eh64->e_shnum; ++i) {
		if (read(fd, &es64, sizeof(es64)) == -1)
			err(4, "read");
		if ((cur = lseek(fd, 0, SEEK_CUR)) == -1)
			err(4, "lseek");
		if (strcmp(shstrtab + es64.sh_name, STABSTR) == 0) {
			if ((stabstr = (char *)malloc(es64.sh_size)) == NULL)
				err(4, "malloc");
			if (lseek(fd, es64.sh_offset, SEEK_SET) == -1)
				err(4, "lseek");
			if (read(fd, stabstr, es64.sh_size) == -1)
				err(4, "read");
		} else if (strcmp(shstrtab + es64.sh_name, STABS) == 0) {
			memcpy(&es64_stab, &es64, sizeof(es64));
		}
		if (lseek(fd, cur, SEEK_SET) == -1)
			err(4, "lseek");
	}

	if (stabstr == NULL)
		err(8, "No stab string section found");

	printf("%-20s %-8s %-8s %-8s %s\n",
	    "Type", "Desc", "Other", "Value", "Name");

	/* Locate the stab section */
	if (lseek(fd, es64_stab.sh_offset, SEEK_SET) == -1)
		err(4, "lseek");
	for (i = 0; i < es64_stab.sh_size / sizeof(stab); ++i) {
		read(fd, &stab, sizeof(stab));
		printf("%-20s %-8d %-8d %08x %s\n",
		    types[stab.n_type], stab.n_desc, stab.n_other,
		    stab.n_value, stabstr + stab.n_strx);
	}

	free(shstrtab);
	free(stabstr);
}

void readstab32(int fd, Elf32_Ehdr *eh32)
{
	Elf32_Shdr es32, es32_stab;
	unsigned int i;
	char *shstrtab = NULL, *stabstr = NULL;
	struct stab stab;

	if (lseek(fd, eh32->e_shoff, SEEK_SET) == -1)
		err(4, "lseek");
	/* Locate section header string table (.shstrtab) first */
	if (lseek(fd, eh32->e_shentsize * eh32->e_shstrndx, SEEK_CUR) == -1)
		err(4, "lseek");
	if (read(fd, &es32, sizeof(es32)) == -1)
		err(4, "read");
	if ((shstrtab = (char *)malloc(es32.sh_size)) == NULL)
		err(4, "malloc");
	if (lseek(fd, es32.sh_offset, SEEK_SET) == -1)
		err(4, "lseek");
	if (read(fd, shstrtab, es32.sh_size) == -1)
		err(4, "read");
	/* Relocate to the start of section header table */
	if (lseek(fd, eh32->e_shoff, SEEK_SET) == -1)
		err(4, "lseek");

	/* Load the stab string section */
	for (i = 0; i < eh32->e_shnum; ++i) {
		if (read(fd, &es32, sizeof(es32)) == -1)
			err(4, "read");
		if (strcmp(shstrtab + es32.sh_name, STABSTR) == 0) {
			if ((stabstr = (char *)malloc(es32.sh_size)) == NULL)
				err(4, "malloc");
			if (lseek(fd, es32.sh_offset, SEEK_SET) == -1)
				err(4, "lseek");
			if (read(fd, stabstr, es32.sh_size) == -1)
				err(4, "read");
		} else if (strcmp(shstrtab + es32.sh_name, STABS) == 0) {
			memcpy(&es32_stab, &es32, sizeof(es32));
		}
	}

	if (stabstr == NULL)
		err(8, "No stab string section found");

	printf("%-20s %-8s %-8s %-8s %s\n",
	    "Type", "Desc", "Other", "Value", "Name");

	/* Locate the stab section */
	if (lseek(fd, es32.sh_offset, SEEK_SET) == -1)
		err(4, "lseek");
	for (i = 0; i < es32.sh_size / sizeof(stab); ++i) {
		read(fd, &stab, sizeof(stab));
		printf("%-20s %-8d %-8d %08x %s\n",
		    types[stab.n_type], stab.n_desc, stab.n_other,
		    stab.n_value, stabstr + stab.n_strx);
	}

	free(shstrtab);
	free(stabstr);
}

void readstab(char *fname)
{
	int fd;
	union {
		Elf32_Ehdr eh32;
		Elf64_Ehdr eh64;
	} eh;
	off_t stab, stabstr;
	size_t stab_size, stabstr_size;

	if ((fd = open(fname, O_RDONLY)) == -1)
		err(2, "open");

	if (read(fd, &eh, sizeof(eh)) == -1)
		err(4, "read");

	if (strncmp(eh.eh32.e_ident, ELFMAG, SELFMAG) != 0)
		errx(8, "%s: not a valid ELF file\n", fname);

	switch (eh.eh32.e_ident[EI_CLASS]) {
	case ELFCLASS32:
		lseek(fd, sizeof(eh.eh32), SEEK_SET);
		elf32 = 1;
		break;
	case ELFCLASS64:
		lseek(fd, sizeof(eh.eh64), SEEK_SET);
		elf32 = 0;
		break;
	default:
		errx(8, "%s: unknown ELF class\n", fname);
		break;
	}

	switch (eh.eh32.e_ident[EI_DATA]) {
	case ELFDATA2LSB:
		elf_little_endian = 1;
		break;
	case ELFDATA2MSB:
		elf_little_endian = 0;
		break;
	default:
		errx(8, "%s: unknown data encoding\n", fname);
		break;
	}

	if (elf32)
		readstab32(fd, &eh.eh32);
	else
		readstab64(fd, &eh.eh64);

	close(fd);
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		usage(argv[0]);

	readstab(argv[1]);

	return 0;
}
