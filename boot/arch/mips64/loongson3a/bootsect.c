
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

void __attribute__((noreturn)) _boot(int (*open)(const char *, int),
    void (*close)(int),
    long (*read)(int, void *, unsigned long),
    unsigned long (*lseek)(int, unsigned long, int))
{
	int i, phnum;
	int fd = open("wd0", 0);
	entry_t entry;

	unsigned int sector_start;
	unsigned long pos, kernel_file_start;

	unsigned char *buf = (unsigned char *)BUFFER;

	unsigned char *part1 = ((unsigned char *)_boot) + PART1_ENTRY;

	sector_start = *(unsigned int *)(part1 + PART_SECTOR_START);
	kernel_file_start = pos = (unsigned long)sector_start * SECTOR_SIZE;

	lseek(fd, pos, 0);
	read(fd, buf, sizeof(struct elf64hdr));
	pos += sizeof(struct elf64hdr);

	if (*(unsigned int *)buf != ELFMAGIC)
		goto fail;

	phnum = ((struct elf64hdr *)buf)->e_phnum;
	entry = (entry_t)(((struct elf64hdr *)buf)->e_entry);

	for (i = 0; i < phnum; ++i) {
		lseek(fd, pos, 0);
		read(fd, buf, sizeof(struct elf64_phdr));
		pos += sizeof(struct elf64_phdr);
		if (((struct elf64_phdr *)buf)->p_type == PT_LOAD) {
			lseek(fd, kernel_file_start +
			    ((struct elf64_phdr *)buf)->p_offset,
			    0);
			read(fd,
			    (char *)(((struct elf64_phdr *)buf)->p_vaddr),
			    ((struct elf64_phdr *)buf)->p_filesz);
		}
	}

	(*entry)();
fail:
	asm volatile ("break 8");
	for (;;)
		/* nothing */;
}
