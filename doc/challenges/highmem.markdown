# High memory mechanism

[*High memory*](https://en.wikipedia.org/wiki/HighMem) (or *HighMem* in short)
is a workaround for kernel to access higher physical addresses.  It is used by
Linux for ensuring total control of the whole physical address space in case
of:

1. The physical address space exceeds the virtual one in size.  In real world
this is addressed by Physical Address Extension (PAE) which is a somewhat
similar idea.  Of course, in real world, you're unlikely to have a 32-bit
computer with a 8GB RAM.
2. Devices taking sooooo much physical address space that direct memory mapping
is infeasible.  `ioremap` in Linux which shares some aspects with HighMem deals
with this situation.  Again, it's very unlikely.
3. The operating system designer insists to do so.

BSD seems to ignore HighMem altogether.  Maybe they didn't care about these
dire cases.

We didn't implement HighMem in this project, and leave it to those who are
interested and passionate as an additional
exercise/challenge/bonus/surprise/trouble/whatever.

### Problem

### Solution

### Interfaces
