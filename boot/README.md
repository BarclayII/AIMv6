# Booting

Booting process, which deals with associated hardware, is inevitably
machine-specific.  Here are the booting processes on different machine
configurations available to us.

### i386/QEMU

The only thing we care is bootloader since most work is done by SeaBIOS.

### arm/Zedboard

TODO: add summary

### mips32/Loongson-2H3A

[Hardware page](http://www.loongnix.org/index.php/3A2H%E5%BC%80%E5%8F%91%E6%9D%BF)

Booting on Loongson 2H is extremely complex.  Current workaround is to make
a bootstrapper for the machine to download via TFTP.  The bootstrapper
does the job of loading and executing MBR.

After finishing the kernel we may

* Crop PMON source code to produce a smaller, cleaner ROM, or
* Modify PMON to support our operating system

if we have time.

The original PMON follows UEFI specifications, which is arguably a better
solution than legacy BIOS as it usually contains builtin supports for file
systems, network, etc.  Our bootstrapper loads MBR rather than directly
finding the kernel because we want to match the behavior of legacy BIOS.

BIOS interrupt calls or similar mechanisms are simulated by branch-and-linking
to preset RAM addresses since there's no such mechanism on MIPS architectures.
This usually means that the ROM should copy itself to RAM after initializing
memory, caches and TLB.

### MBR

The MBR records four main partitions:

* The first one is reserved for an existing commonly-used partition such as FAT
(for Zedboard) or ext2 (shipped with Loongson-2H3A) due to compatibility.
* The second one is a small partition with boot flag and an "unknown" partition
type (one can invoke `fdisk(8)` to view the list of known partition types),
containing our kernel.
* The third one is our customized file system.  Again, its type is an unknown
one, different from the second partition.
