# MIPS Lab 4 HOWTO - (No Longer a Step by Step) Walkthrough

## Overview

Lab 4 deals with physical memory allocation.

## MSIM configuration changes

We're going to extend the main memory of the MSIM simulator
to 2GB.

As MIPS specification required, physical address `0x00000000`
to `0x0fffffff` corresponds to the lowest 256MB RAM, and
`0x10000000` to `0x1fffffff` corresponds to the devices.

Therefore, the main memory should be separated into two parts:
one for the lowest 256MB RAM (we call it `LowRAM` here), another
for the remaining 1792MB RAM (we call it `HighRAM` here).

So we're going to add another piece of main memory at some higher
address, say, `0x90000000`.  A benefit of choosing `0x90000000` is
that physical addresses `0x90000000` to `0xffffffff` exactly map
to `HighRAM`.

```
add rwm xram 0x90000000
```

We also want to specify the size, by either:

- Directly allocate from memory:
```
rwm generic 1792M
```
  or,
- Using memory mapping technique to read-write on a file instead,
  if your memory is scarce:
```
rwm fmap "mem.img"
```
  You should create an empty 1792M file `mem.img`, though.

## MIPS virtual address layout

There are 32-bit and 64-bit variants of MIPS virtual address layout.

### MIPS32 variant

```
+-----------------------+ 0xffffffff
|        KSEG3          |
|       Mapped          |
|                       |
+-----------------------+ 0xe0000000
|     KSEG2/KSSEG       |
|       Mapped          |
|                       |
+-----------------------+ 0xc0000000
|        KSEG1          |
|      Unmapped         |
|      Uncached         |
+-----------------------+ 0xa0000000
|        KSEG0          |
|      Unmapped         |
|       Cached          |
+-----------------------+ 0x80000000
|                       |
|                       |
|                       |
|                       |
|                       |
|        USEG           |
|                       |
|                       |
|       Mapped          |
|                       |
|                       |
|                       |
|                       |
|                       |
|                       |
+-----------------------+ 0x00000000
```

### `KSEG0` and `KSEG1` revisited

Remember the address translation for Low RAM in Lab 1?  The real name
for those two virtual address spaces are `KSEG0` and `KSEG1`.

You should be already familiar with translations between Low RAM and
`KSEG0` or `KSEG1`.

### Unmapped and Mapped

The term *unmapped* means that the virtual address is not translated via
a *memory management unit* (MMU).  *Mapped* is just the opposite.

In Mapped translation, the paging mechanism is always on.  That is, the
processor finds the physical page according to the virtual page the
address is on via a MMU, and adds the page offset, to obtain the physical
address.

### How's a MMU look like?

(Un)fortunately, the MMU on MIPS processor is ridiculously simple:
it's a mere TLB, with some coprocessor registers controlling it.

Basically, when the processor come across a virtual address, the processor
looks it up in the TLB.  If it's found, the processor happily accesses memory
or devices with physical address obtained.  If the processor can't find the
virtual address in TLB, or the virtual address is marked invalid there, it
simply **throws an exception**.

#### Multiple matching entries?

As TLB is managed by software on MIPS, unlike most architectures, a buggy
software may write two entries with the same virtual page number (VPN) to TLB.
This may shutdown
the TLB altogether, or even cause severe damage to TLB on early MIPS
processors.  Although hardware damage won't happen any more on modern
processors, multiple matching entries may still cause undefined behavior,
and therefore should be avoided.

### TLB refill exceptions

The exceptions thrown by the processor whenever it can't find a valid
virtual address entry in the TLB are called *TLB refill exceptions*.  When it
happens on loading, the *TLB loading miss* exception (code 2) is thrown.
A *TLB storing miss* exception (code 3) is thrown if it happens on a store
instruction.

When a TLB refill exception is thrown, the exception handler entry is
`EBase + 0x0`, as opposed to `EBase + 0x180` for generic exception handler.
This is mainly because TLB refill exceptions happen so frequently that they
deserve a separate, compact, efficient handler, usually written in assembly.

With TLB refill exceptions, MIPS processors effectively pass the paging
scheme to software.

### How does a MIPS TLB entry look like?

A MIPS TLB entry actually holds a pair of physical-virtual memory mappings:

1. *Virtual page number* (VPN).  It's always an even number as one entry
  corresponds to two mappings, one for virtual page number `VPN`, another
  for `VPN+1`.
2. *Even physical frame number* (PFN0), the physical frame number (PFN)
  for `VPN`.
3. *Odd physical frame number* (PFN1), the PFN for `VPN+1`.
4. Flags, including
    - *Global* (G), see below.
    - *Dirty* (D0 and D1 for even/odd page), which allows writing.
    - *Valid* (V0 and V1 for even/odd page), which marks the validity.
    - *Cache coherency field* (C0 and C1 for even/odd page), 2 for uncached,
      3 for coherently cached.  Usually it's 3 for RAM, and 2 for peripheral
      devices.
5. *Address space ID* (ASID).  When processor come across a virtual address,
  it not only checks the VPN, but also checks whether
    - the current ASID in a coprocessor register `ENTRYHI` matches the one
      in the TLB entry, or
    - the G bit in the TLB entry is set.
  Usually ASID is ignored, and modern operating systems always sets G bits.
6. *Page mask*, indicating the page size for one TLB entry, to handle
  variably-sized pages.
    - Usually it's untouched, meaning that the page size is always 4KB.

### Controlling TLB

MIPS architecture defines four basic TLB instructions:

1. `TLBP` - Probe the TLB for a matching entry.
2. `TLBR` - Read the TLB by index.
3. `TLBWR` - Write an entry into TLB indexed by `RANDOM` register.
4. `TLBWI` - Write an entry into TLB indexed by `INDEX` register.

It also defined several coprocessor registers for TLB management:

1. `INDEX`
    - A read-write coprocessor register
2. `RANDOM`
    - A self-increasing register looping from the content of `WIRED` to
      the number of TLB entries minus 1.
3. `ENTRYLO0` and `ENTRYLO1`
    - Bit 0 is the Global bit.
    - Bit 1 is the Valid bit.
    - Bit 2 is the Dirty bit.
    - Bit 3 to 5 is a field indicating cache coherency.
        + For RAM accesses where caches could be exploited for accelerating
          access speed, the field value is usually 3 (i.e. bit 4 and 3 set,
          bit 5 clear).
        + For peripheral device accesses where the contents could change by
          their own, the field value is usually 2 (i.e. bit 4 set, bit 5 and
          3 clear), to avoid using caches to access this virtual address.
    - Bit 6 and beyond is a field containing the PFN.
4. `PAGEMASK`
    - A register containing bit masks indicating which bits should not
      participate in TLB matching.
5. `WIRED`
    - Defines the lower bound of the range of `RANDOM` register.  A non-zero
      `WIRED` register keeps the first few entries from being touched by
      `TLBWR` instructions.
6. `BADVADDR` (not specific for TLB, though)
    - Contains the virtual address causing Address Error exceptions and TLB
      exceptions, including refill exceptions and TLB modified exception
      (discussed in later labs).
7. `ENTRYHI`
    - Contains the virtual address, excluding the page offset plus the lowest
      bit of VPN, since a TLB entry corresponds to a pair of adjacent virtual
      pages.
    - The lowest 8 bits hold the ASID discussed above.
8. `CONFIG1` (does not exist on Revision 1)
    - A `MMU` field contains the number of TLB entries minus 1.

The format of registers are available in MIPS Architecture Volume III.

We can see that `ENTRYLO0`, `ENTRYLO1`, `ENTRYHI` and `PAGEMASK` registers
exactly covers the content of a whole TLB entry.

MIPS processors also have a `CONTEXT` register for supposedly easier access
to the page table, but it's not that mandatory in operating system development.

#### `TLBP`

`TLBP` instruction probes the TLB for a VPN (possibly with an ASID) given in
`ENTRYHI` register, and returns the index in `INDEX` register.

If there's no such entry, `INDEX` would be negative.

#### `TLBR`

`TLBR` loads `ENTRYLO0`, `ENTRYLO1`, `ENTRYHI`, `PAGEMASK` registers with
TLB entry contents.  The entry is pointed to by the `INDEX` register.

#### `TLBWR` and `TLBWI`

`TLBWR` writes `ENTRYLO0`, `ENTRYLO1`, `ENTRYHI`, `PAGEMASK` registers into
TLB indexed by `RANDOM` registers.

`TLBWI` does mostly the same except that the index is given by `INDEX` register.

### A simple memory mapping

**NOTE**: this mapping is only a demonstration and not very well-suited in our
case.

Here, we assume that we want to map physical address space
`0x00000000 - 0x7fffffff` to
virtual address space `0x00000000 - 0x7fffffff`, which is an identical mapping.
We also want the virtual pages to be 4KB-sized and writable.

After a TLB refill exception is thrown, the VPN excluding the lowest bit is
stored in `ENTRYHI` register, so we should retrieve it first (noting that
only `k0` and `k1` is available if other registers are not saved)

```
LEAF(tlb_refill)
        j       __tlb_refill
END(tlb_refill)

__tlb_refill:
        MFC0    k0, CP0_ENTRYHI
```

We should also clear the lowest 8 bits, assuming we're not using ASID:

```
LEAF(tlb_refill)
        j       __tlb_refill
END(tlb_refill)

__tlb_refill:
        MFC0    k0, CP0_ENTRYHI
        or      k0, 0xff
        xor     k0, 0xff
```

Since the PFN is identical to VPN, the process for writing to `ENTRYLO0`
is simple, as we only need to store the VPN into corresponding field, and
set up the flag bits:

```
LEAF(tlb_refill)
        j       __tlb_refill
END(tlb_refill)

__tlb_refill:
        MFC0    k0, CP0_ENTRYHI
        or      k0, 0xff
        xor     k0, 0xff
        SRL     k1, k0, 6
        or      k1, TLB_CACHE_UNCACHED | TLB_VALID | TLB_DIRTY | TLB_GLOBAL
        MTC0    k1, CP0_ENTRYLO0
```

After setting up the even virtual page, we now turn to odd page by writing
to `ENTRYLO1`:

```
LEAF(tlb_refill)
        j       __tlb_refill
END(tlb_refill)

__tlb_refill:
        MFC0    k0, CP0_ENTRYHI
        or      k0, 0xff
        xor     k0, 0xff
        SRL     k1, k0, 6
        or      k1, TLB_CACHE_UNCACHED | TLB_VALID | TLB_DIRTY | TLB_GLOBAL
        MTC0    k1, CP0_ENTRYLO0
        SRL     k1, k0, 6
        ADD     k1, 1 << 6
        or      k1, TLB_CACHE_UNCACHED | TLB_VALID | TLB_DIRTY | TLB_GLOBAL
        MTC0    k1, CP0_ENTRYLO1
```

Finally we write to TLB with these records and return:

```
LEAF(tlb_refill)
        j       __tlb_refill
END(tlb_refill)

__tlb_refill:
        MFC0    k0, CP0_ENTRYHI
        or      k0, 0xff
        xor     k0, 0xff
        SRL     k1, k0, 6
        or      k1, TLB_CACHE_UNCACHED | TLB_VALID | TLB_DIRTY | TLB_GLOBAL
        MTC0    k1, CP0_ENTRYLO0
        SRL     k1, k0, 6
        ADD     k1, 1 << 6
        or      k1, TLB_CACHE_UNCACHED | TLB_VALID | TLB_DIRTY | TLB_GLOBAL
        MTC0    k1, CP0_ENTRYLO1
        tlbwr
        eret
```

During trap initialization, the content of TLB handler should be copied to
the exception vector:

```C
extern unsigned char tlb_refill[];

memcpy(EXCEPT_TLB, tlb_refill, EXCEPT_HANDLER_SIZE);
```

### TLB initialization

As TLB content is usually undetermined after being powered on, one should
initialize TLB somewhere before using it.

A typical TLB initializer fills up every TLB entry with distinct, "dummy"
VPNs, and
the V-bit cleared, so as to throw an exception to require the software
to fill in meaningful context whenever those "dummy" VPNs are accessed.

The TLB initializer could be used to "flush" the TLB between context
switches (i.e. switching from user to kernel, or switching between
processes) as well.

##### Programming task

1. Design a kernel memory mapping to address all 2GB RAMs however you like.
    - The mapping could be either static (via simple rules) or using a
      hierarchical page table.  It's flexible since TLB filling is performed
      by software.
2. Implement the TLB refill handler, and modify the trap initialization routine
  to take your TLB refill handler.
3. Implement the TLB initializer function, and invoke it somewhere before
  TLB is being used.

It's even better to have your TLB handler tested.

### MIPS64 variant

MIPS64 defined and extended several address spaces for addressing more than
32 bit of physical addresses.

In particular, the introduction of `XKPHY` address space guarantees
unmapped one-to-one mapping from virtual address to physical address.  This
makes addressing physical space in kernel extremely easy.

The details can be found in MIPS64 Architecture Volume III.

##### (Definitely Optional) Challenge

Suppose that a particular MIPS32 machine has the following physical address
layout:

1. `0x00000000 - 0x0fffffff`: 256MB Low RAM.
2. `0x10000000 - 0x1fffffff`: Devices, including BIOS.
3. `0x20000000 - 0x3fffffff`: Advanced devices including PCI, AHCI, ACPI, etc.
4. `0x40000000 - 0x8fffffff`: 1.25G VGA RAM.
6. `0x90000000 - 0xffffffff`: 1792MB High RAM.  2G RAM in total.

There, no physical address is idle, and the kernel needs to address the full
4G physical address space.

The problem is, with `KSEG0` and `KSEG1` mapped to the same physical address
range (`0x00000000 - 0x1fffffff`), it's impossible to establish a one-to-one
mapping as above.

Redesign and reimplement the kernel memory mapping mechanism to solve the
problem.

There's quite a lot of work you're doing here.  It's related to

1. HighMEM.
2. `ioremap` in Linux.
3. *Physical address extension* (PAE).

## Physical Page Allocator

Here we're going to make a physical page allocator to allocate continuous page
blocks.

In particular, the allocator should provide at least two primitive operations:

1. `struct page *alloc_pages(int n)`, for allocating physically-continuous
  pages.
2. `void free_pages(struct page *p)`, for releasing an allocated page block.

where `struct page` is a data structure maintaining a physical page (or a
physical page block).  *The name could be changed to suit your purpose*.

##### Programming task

1. Implement a physical page allocator.
    - You should also provide mechanisms to actually access those physical
      pages.

This is a huge task compared to labs before, so teamwork is probably important.

You could also discuss with other groups to struggle for a better design.
