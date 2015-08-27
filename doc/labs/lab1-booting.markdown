# Lab 1: Booting

Booting is the very first process for a machine to startup, which can be
roughly separated into two stages:

1. The CPU begins execution from BIOS or other ROM storage manufactured by
  OEM, performing early necessary hardware discovery (such as disk storage,
  serial, VGA, etc.) and self-tests. Normally this process is largely
  unchangeable except for a small set of configurations available through an
  interface.
2. After early discovery and self-tests, the program residing in BIOS or other
  ROM storage transfers control to a program called *bootloader*, which is
  stored on external storage such as hard disks, CD-ROMs, flash drives, while
  providing interfaces for convenient interactions with hardware such as BIOS
  interrupts.

The installation of an operating system has the responsibility of properly
setting up a bootloader.

In this lab, we'll focus on MBR, a legacy booting scheme which is widely
available (and sometimes the only option) on most machines.

### MBR

Starting from PC-DOS 2.0 running on IBM PC with x86 instruction set, the
bootloader is installed on the first sector of bootable external storage,
which is called the *Master Boot Record* (MBR).

The structure of MBR is described below:

```C
#pragma pack (1)

struct chs_address {				/* cylinder-head-sector */
	unsigned char	head;			/* head */
	unsigned char	sector:6;		/* sector */
	unsigned char	cylinder_high:2;	/* cylinder bit 9-8 */
	unsigned char	cylinder_low;		/* cylinder bit 7-0 */
};	/* little-endian */

struct partition_entry {
	unsigned char	status;		/* partition status */
#define BOOTABLE	0x80
#define INACTIVE	0x00
	/*
	 * In CHS address, sectors start from 1 by convention, and the range
	 * of head is 0-254.
	 * 
	 * Usually CHS is insufficient to express a sector's address, in
	 * which case the values are set to maximum.
	 */
	struct chs_address chs_first;	/* CHS address of first sector */
	unsigned char	type;		/* partition type code */
	struct chs_address chs_last;	/* CHS address of last sector */
	unsigned int	sec_first;	/* No. of first sector */
	unsigned int	sec_count;	/* Number of sectors */
};

struct mbr {
	unsigned char	code[446];	/* bootloader code */
	struct partition_entry pe[4];	/* 4 primary partitions */
	unsigned char	signature[2];	/* always 0x55 0xaa */
};
```

The BIOS or other ROM storage loads the `code` member of MBR, that is, the
first 446 bytes of the first sector on the external storage, into memory.

Early bootloaders could fit themselves into the 446 bytes, but modern ones
like GRUB certainly couldn't do so as they have to support lots of features.
In this case, the 446 bytes contain only the *first stage* of the bootloader,
whose purpose is mainly to load the *second stage* code stored elsewhere into
memory and execute it.

In our experiments, the bootloader code could **always** fit themselves into
446 bytes, regardless of architecture.

##### i386

(TODO: add i386 bootloader code explanation)

##### armv7a-le

(TODO: add armv7a-le bootloader code explanation)

##### MIPS64

The original PMON, the BIOS for Loongson board, also took the role of
bootloader (and therefore does not allow custom bootloader installation).
We modified PMON to provide four helper functions and values, and to actually
load and execute the code in MBR instead of finding the kernel directly:

1. `int fd`
  * An integer representing handle of the hard disk.
2. `void findsect(int fd, unsigned int sec)`
  * A function for locating the `sec`-th sector from start.
3. `int read(int fd, void *buf, unsigned int len)`
  * Identical to `read(2)` function.
4. `int lseek(int fd, unsigned int offset, int whence)`
  * Identical to `lseek(2)` function.

The function and values are passed as arguments for the following bootloader
entry function:

```C
void boot(fd, findsect, read, lseek)
```

The bootloader code is in
[bootsect.c](../../boot/arch/mips64/loongson3a/bootsect.c).
After compilation, the function code itself is copied to the first 446 bytes
of the first sector.

PMON loads the entire MBR into memory starting at address 0x80100000, which
becomes the entry address of bootloader code (`boot()` function).  The
partition entries could then be referred by simply adding an offset to the
`boot()` function.

We designed the bootloader to find the kernel, stored as ELF format, on the
second partition.

The bootloader does the following:

1. Find and locate the starting sector of second partition
  (`bootsect.c:63-83`).
2. Read the ELF header there
  (`bootsect.c:86-89`).
3. Find the number of program segments as well as the entry address
  (`bootsect.c:91-95`).
4. For each program segment, check if the segment is loadable as indicated
  in the *segment header* (also known as the *program header*)
  (`bootsect.c:98-108`).
  If the segment is indeed loadable, load the segment into memory at the
  address given in the segment header.
5. Jump to entry and begin execution of kernel.

Because the encoding efficiency of MIPS instruction set is not as effective
as that of i386, 446 bytes (that is, 111 instructions) could not do much job.
We omitted verification, which is usually a vital process, to avoid introducing
the second stage.

### Restrictions of MBR

From knowledge above, we could find that legacy MBR method has the following
restrictions:

1. **Limited (primary) partitions**, although *extended partition* type, which
  can hold several logical partitions, is introduced to resolve this issue.
2. **Limited disk size**, the 4-byte limit of sector count limits the disk
  size to be at most 2TB.
3. **Weak bootloader**, or even impossible to implement a bootloader without
  fine-grained help from BIOS on some RISC architectures.

All these issues called for a new bootloading solution and a new partition
scheme.  *Unified Extensible Firmware Interface* (UEFI) as well as *GUID
Partition Table* (GPT) are thus introduced.

### Exercises

Assuming the repository is already cloned, and the current directory is
the repository directory.

1. Switch branch to `lab1`.
2. Read and understand `Makefile` first.
3. After understanding `Makefile`, change configuration accordingly.
4. Compile with your choice of architecture by
    make ARCH=<your-architecture>
5. Install to external storage by
    make ARCH=<your-architecture> install
  One may have to plug in the external storage (SD card or SATA hard disk).
  Make sure the target device path is correct.
6. Open a serial and connect to the box.
7. Bring up the machine to see if you have correctly compiled and installed
  the kernel and bootloader.
8. Read the bootloader code (both C code, assembly code, and disassembly
  generated by `objdump`).  Kernel code can be ignored at present.

###### i386

(TODO)

###### armv7a-le

(TODO)

###### MIPS64

Loongson 3A box should emit a line
    Hello from kernel!
and become idle.
