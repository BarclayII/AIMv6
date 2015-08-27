# Booting

Booting process, which deals with associated hardware, is inevitably
machine-specific.  Here are the booting processes on different machine
configurations available to us.

### i386/QEMU

The only thing we care is bootloader since most work is done by SeaBIOS.

### arm/Zedboard

Booting on zedboard includes multiple stages.

* Stage 0, on-chip bootROM, poorly documented, possibly executed in-place.
Programmers have NO access to this ROM. It writes basic configuration to the
processing system (PS) but not programmable logic (PL), making basic peripherals
accessible, loads boot image from media (according to jumper values), and handoff
to first-stage bootloader (FSBL) inside the image.
* Stage 1, FSBL, loaded into OCM. Should program PS and PL (even if it is blank),
load next stage and handoff.
* Stage 2, SSBL or second-stage bootloader, loaded into DDR. Should find kernel,
load it and boot it.

Our design is to build a basic FSBL, which reads MBR on SD card, verify its magic,
and handoff. This FSBL should eventually end up in QSPI flash to save a boot flag
(Because bootROM's exact behavior is undocumented), but currently a file in the
FAT32 partition is just okay.

Our SSBL should start from MBR, and from this point on everything should be unified
across platforms and devices.

### mips64/Loongson3A

Loongson Tech provided a box dubbed 3A-MATX with PMON code shipped
at (TODO: upload PMON patch and source code, which isn't included on Loongson's
website).  I modified the `main` subroutine of PMON to load and jump to MBR.

The bootstrapper code in MBR demands extreme care, and is nearly-impossible to
code without existing helper functions (such as `open`, `read`, `puts`, etc.,
which is already provided in PMON, which made me ultimately decide to modify
PMON despite its ~~crappy~~ unfavorable code style instead of writing my own
from scratch).

### MBR

The MBR records four main partitions:

* The first one is reserved for an existing commonly-used partition such as FAT
(for Zedboard) or ext2 (shipped with Loongson-3A) due to compatibility.
* The second one is a small partition with boot flag and an "unknown" partition
type `0xd0` (one can invoke `fdisk(8)` to view the list of known partition
types), containing our kernel.
* The third one is a fairly-sized partition which is for swapping.  Its type
is again a different "unknown" one `0xd1`.
* The fourth one is our customized file system.  its type is an another unknown
one `0xd2`, different from the second and third partition.

