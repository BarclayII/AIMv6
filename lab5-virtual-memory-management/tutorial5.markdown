# Lab 5 - A brief walkthrough

## Overview

Lab 5 does several things:

- Establish a multi-level hierarchical page table mechanism.
- Implement the SLAB allocator.

## Hierarchical Page Table

The hierarchical page table is quite straightforward for virtual-physical
page translation.

Here we assume that you're already familiar with the two-level hierarchical
page table (that is, the *page directory entry*, or PDE, and *page table entry*,
or PTE, something like that).

##### Paper exercise

How many levels are needed on a 64-bit processor with 48-bit physical addresses
(that is the case of AMD64 and Loongson 3A), if the page size is still 4KB?

### Possible primitives and interfaces

**NOTE:** this section is only a suggestion.  Other schemes are possible.

Manipulating the hierarchical page table involves possibly the following
primitives:

1. Creating a page table.
2. Creating a page table entry given the virtual page, physical page, and the
  page table.
    - The virtual page could be embedded in the virtual address itself, or be
      explicitly specified as an argument.  This is a (rather unimportant)
      design choice.
    - The physical page could be the page structure (`struct run` in xv6, or
      something like `struct page`), the PFN itself, or a combination of
      page block structure and an offset.  Again, this is a design decision.
    - The page table is usually represented by a pointer pointing to the
      virtual page containing the page directory.
    - The PTE containing the PFN as well as other flags are to be filled into
      the `ENTRYLO0` and `ENTRYLO1` registers after some sort of transformation,
      such as simple arithmetic operations (e.g. bitwise rotation).
    - See xv6 source code and documentation for how to organize a page table on
      i386.  This would probably inspire your implementation on MIPS32.
3. Remove a page table entry from the page table, given the virtual page and/or
  the physical page.
4. Change the permission flags of a page table entry.

The following conversion routines are probably needed as well:

1. Page structure to/from physical address
2. Physical address to/from kernel virtual address
    - The kernel virtual address is usually different from that of user virtual
      address, which relates to the page table we're using.
3. User virtual address to physical address (or the page table walker)

### MIPS hazard: cache aliasing

MSIM didn't simulate a cache, so you can skip this section if you don't plan to
migrate to Loongson.

Typical MIPS processor uses a virtually-indexed, virtually-tagged primary cache,
which may cause serious problems in software.  To further explain this, we need
to review the concept of a cache.

#### Cache revisited

A typical cache decomposes the physical address into several fields: *tag*,
*index*, and *offset*.  For a, say, 64KB 4-way cache with block size 32bit, the
cache itself looks like the following:

```
(Each cell in the content field holds a byte.  The content field should exactly
hold a block, which is 4 bytes in this case)
(Also, flags such as dirty bit and valid bit are omitted in the following...
ASCII art?)

     Tag     Content     Tag    Content     Tag    Content     Tag    Content
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
0  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
1  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
2  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
3  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
4  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
5  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |      |  |  |  |  |
   +------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+------+--+--+--+--+
...
(All the way to 2^12 = 4096 rows)
```

Since there are 4096 rows, the cache needs a 12-bit field for locating the row,
and an additional 2 bit for addressing which cell the cache will take.
The rest of bits comprises the *tag* field, which is used to compare with the
tag field stored in the cache.

- If a matching tag is found, the cache will
  take the value according to index (the row number), and the offset
  (the cell number), in the item with the matching tag.
- If there's no such tag, one item on the indexed row would be replaced by
  the requested content from the main memory (or upper-level storage, e.g.
  secondary caches).

If the cache above is on a 32-bit machine, the cache would decompose the
physical address in the following way:

```
         18              12      2
+------------------+------------+--+
|       Tag        |    Index   |Of|
+------------------+------------+--+
```

##### Paper exercise

Count the bit length of tag field, index field, and offset field for a
64KB, 4-way cache, with 256-bit cache block (holding 4 64-bit words), on a
64-bit machine with 48-bit address lines, which is the case of Loongson 3A.

#### The problem with virtual indices and tags

Since the address lines are 48-bit, the high-order bits (63..48) are unused
in cache operations.  Therefore Loongson would decompose the 64-bit virtual
address into

```
        16                       24                     9        5
+----------------+----------------------------------+---------+-----+
|Unused by  cache|               Tag                |  Index  | Off |
+----------------+----------------------------------+---------+-----+
```

Comparing to the paging hierarchy where the page size is 4KB:

```
        16                       24                     9        5
+----------------+----------------------------------+---------+-----+
|Unused by  cache|               Tag                |  Index  | Off |
+----------------+----------------------------------+--+------+-----+
|Unused  by   MMU|         Page table stuff            |Page  offset|
+----------------+-------------------------------------+------------+
        16                          26                       12
```

This is fine if we're using physical address for caches, since physical address
one-to-one maps to the memory itself.  However, most MIPS processors virtually
index and tag their caches, apparently trying to save the time for accessing a
TLB and/or handling TLB exceptions.  Such "trick" becomes disasterous if
shared memory is involved, i.e. multiple virtual pages are mapped to the
same physical page.

##### Paper exercise

Suppose that we're operating on a Loongson 3A, with its data cache defined
above.  The page size is 4KB.

Consider that there are two virtual addresses:

```C
int *VA1 = (int *)0x1000;
int *VA2 = (int *)0x2000;
```

They're mapped to the same physical page:

```C
unsigned long PA = 0x10000;     /* physical address of that page */
```

The following C code is executed:

```C
*VA1 = 1;                       /* (1) */
kprintf("%d\n", *VA2);          /* (2) */
*VA2 = 2;                       /* (3) */
kprintf("%d\n", *VA1);          /* (4) */
```

Assume that TLB handler and `kprintf()` are working correctly.  Moreover,
assume that the cache is empty prior to (1), `VA1` is always cached after (1),
and `VA2` is always cached after (2).

1. What's the output, if the cache is indexed and tagged by physical address?
2. What's the output, if the cache is indexed and tagged by *virtual* address?
  Such inconsistency is called *cache aliases*.
3. Think of ways to avoid cache aliases.

##### Programming exercise

Design and implement your interfaces for manipulating a page table.

Change your TLB refill handler to test your page table.

## SLAB allocator

SLAB allocator, proposed by Sun Microsystems (which is long dead), and first
used in Solaris, is an arbitrary-size allocator solution to minimize the impact
of both internal and external fragmentation.  It's best suited in kernel object
allocation and deallocation, where lots of instances of same classes (e.g.
process structures, page structures, spinlocks...) are maintained.

### The overall idea

SLAB allocator is usually based on a working physical page allocator.  The idea
of SLAB allocator is that it divides a page (or a contiguous page block) into
equally-sized small chunks.  The page or page block being divided is called
a *slab*.

When serving an allocation request, the allocator tries to find whether a
slab with a suitable size and free chunks is available.  If so, the allocator
returns one free chunk.  If not, the allocator demands a page (or a page block)
from the base page allocator, divide it into chunks (and making the page or
page block a slab), and gives the requester a chunk.

Serving a deallocation request is simple: the allocator adds the chunk back to
the slab, marking the chunk as free.  If the slab is empty (i.e. no chunk is
used by others), the allocator may keep the slab, or return it to the base
page allocator.

##### Programming exercise

1. Design a SLAB allocator yourself.
    - You should take external fragmentation, internal fragmentation, and
      memory efficiency into consideration.
2. Implement a SLAB allocator to allocate/free arbitrarily-sized memory
  chunks efficiently.
    - You should implement `kmalloc()` and `kfree()` function, which are similar
      to the user space cousins `malloc(3)` and `free(3)`.
    - SLAB allocator is generic and applicable to all architectures, whereas
      page table manipulation is architecture-specific.

The primary goal is to work by whatever means.

