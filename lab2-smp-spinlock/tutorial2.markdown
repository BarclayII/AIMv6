# MIPS Lab 2 HOWTO - A Step by Step (?) Walkthrough

## Overview

Lab 2 introduces two tasks.

1. SMP spinup
    - We'll focus on how to spin up multiple processors on MSIM, which simulates
      R4000 processors, with MIPS32 Revision 1 architecture.
    - We're also taking Loongson with MIPS64 Revision 2 into consideration.
2. Synchronization: spinlocks
    - Only MIPS implementation is introduced here.

I planned to talk about trap handling in this lab, but soon I found that it'll
be hard to describe the inter-processor interrupt or multicore related stuff
if you don't understand SMP.  So I'll defer trap handling tutoring to next lab.

### Highlights

1. We have already experienced separating architecture-specific code from
  generic code.  In this lab, the *machine-specific* stuff kicks in.
    - The codebase should efficiently distinguish machine-specific source files
      among the architecture-specific source files.
    - We're dealing with two target machines: MSIM with R4000 (MIPS32), and
      optionally Loongson with Loongson 3A (MIPS64 Revision 2) later.
2. We're going to make the kernel codebase *configurable*, in order to support
  different target machines.

## Materials

1. The headers and source files from Lab 1.
2. The following headers
    - `atomic.h`
    - `smp-arch.h`
    - `smp-mach.h`

## Organizing kernel code: machine-specific stuff

In this lab we're going to deal with machine-specific code, namely routines
related to SMP, since the mechanism may be different among architectures
and machines.

We should make two new directories for holding machine-specific things:

1. `kern/arch/mips/mach-msim`, to hold the source codes.
2. `include/arch/mips/asm/mach-msim/mach`, to hold the header files.
    - Source files should include the headers inside by something like
```C
#include <mach/smp.h>
```

##### Source management exercise

Figure out whether the new headers are generic, architecture-specific, or
furthermore, machine-specific.

- Yeah I know you can even infer by file names, but I recommend you to
  read the code there nonetheless.

Move the new headers into the corresponding directories.

Rename `smp-arch.h` and `smp-mach.h` back to `smp.h` in their directories.

## MSIM configuration changes

In this lab, we'll change the configuration to make multiple CPUs and
inter-processor communication available.

Assuming that your configuration file didn't change from Lab 1:

```
# firmware.conf
add dcpu cpu0
add rwm ram 0x00000000
ram generic 256M
add rom fw 0x1fc00000
fw generic 1M
fw load "firmware.bin"
add dprinter lp0 0x1f000010
# you should had written some lines setting up the disk controller as well.
```

We should add three more CPUs, yielding a quad-core computer:

```
add dcpu cpu1
add dcpu cpu2
add dcpu cpu3
```

By referencing the manual, we should also add a so-called "interprocessor
communication device":

```
add dorder ord0 <your-physical-address> <your-interrupt-mask>
```

*Mailbox registers* are commonly implemented on modern MIPS chips (such as
Loongson, SGI Octane, etc.), one set per processor, to
store some parameters or arguments for inter-processor communication.
Unfortunately, MSIM didn't bring such registers in that "dorder" device, so
we should add them by our own:

```
add rwm mailbox0 <physical-address>
mailbox0 generic <mailbox-size>
add rwm mailbox1 <physical-address + mailbox-size>
mailbox1 generic <mailbox-size>
add rwm mailbox2 <physical-address + mailbox-size * 2>
mailbox2 generic <mailbox-size>
add rwm mailbox3 <physical-address + mailbox-size * 3>
mailbox3 generic <mailbox-size>
```

Note that although `mailboxN` are read-write memory, they should not
be treated as a part of RAM, as they're a component of a whole
inter-processor communication device.

## Configuration headers - making the codebase configurable

If your code has any "magic number", including but not limited to:

1. The physical address of devices, specified in MSIM configuration file,
  or in Loongson 3A documentation, without any particular *logical* reason.
2. The hyperparameters of that particular machine, including number of
  processors, etc.
3. The hyperparameters you wish to expose for configuration by other
  developers or users, e.g. the size of kernel stack, the maximum number of
  processes, the maximum number of total devices, etc.

Then you should probably put them into *configuration headers*, which are
commonly named `config.h`.

##### Programming exercise

Reorganize your code, figure out the magic numbers in your code, and whether
they're machine-specific, architecture-specific, or could be applied to the
whole codebase.

Replace those magic numbers with macros.

For example, if you hardwired the physical address of MSIM printer (such
as 0x1f000010), you could replace it with a macro like `MSIM_LP_BASE`,
defining the macro in a machine-specific configuration header.

- The generic configuration header should be `<config.h>`, where
  the architecture-specific configuration header is included.
- The architecture-specific configuration header should be `<asm/config.h>`,
  where the machine-specific configuration header is included.
- The machine-specific configuration header should be `<mach/config.h>`.

It's entirely up to you to decide what to put inside the configuration headers.

## SMP startup: firmware

After the machine is powered on, all processors start their execution at
address `0xbfc00000`.

Apparently, we need to suspend all but one processor's execution, because
if we leave them as is, the hardware initialization, kernel loading, etc.,
would be done several times, which is undesirable in most cases, and sometimes
even breaks some hardware or cause undefined behavior.

The sole one processor which is allowed execution there is called the
*master* processor, whereas the other suspended processors are called
the *slave* processors.

Each processor almost always has a unique number assigned by hardware.  We
call those unique numbers *CPU identifiers*.

It's common practice to simply elect the one with the smallest CPU ID
(usually 0) as the master processor, making others slave processors.

In our lab, if you include `<asm/smp.h>`, which should be originally
the given `smp-arch.h`, you can obtain the CPU ID by

- `cpuid()` in C code, or
- `cpuid` pseudo-instruction in assembly code.  The syntax looks like:

```
cpuid   <result-register>, <temporary-register>
```

The problem is then how to implement the pseudo-instruction `cpuid` and the
C macro `cpuid()`.

Unfortunately, how to obtain such CPU identifiers are different between
MIPS revision 1, and revision 2 or later.

### Obtaining CPU ID on MIPS Revision 2 and later

MIPS32/64 Revision 2 and later defined a coprocessor register `EBASE`,
whose last 10-bits stores the CPU ID.

Therefore, the `cpuid()` macro can be easily implemented as something
like

```C
unsigned int cpuid(void)
{
        return read_c0_ebase() & 0x3ff;
}
```

The assembly code for putting CPU ID inside register `v0` is also quite straight
forward:

```
MFC0    v0, CP0_EBASE
andi    v0, 0x3ff
```

#### Instruction set macros

You may frequently see something like `_MIPS_ARCH_xxxx` then.  This is a
compiler-defined macro based on the compiler options you gave.

For example, if you specify the `-march=mips32` compiler option, `gcc(1)` would
define the macro `_MIPS_ARCH_MIPS32`.  Likewise, `-march=loongson3a` would make
`gcc(1)` define the macro `_MIPS_ARCH_LOONGSON3A`.

Since `-mips32` compiler option is an abbreviation of compiler option
`-march=mips32` (see `gcc(1)`), specifying `-mips32` would also define the
macro `_MIPS_ARCH_MIPS32`.

#### `__ASSEMBLER__` macro

The `__ASSEMBLER__` macro is automatically defined if the source is an
assembly source, or the header is included by an assembly source.

One can use the following compiler directives to mix assembler
codes and C codes inside one (header) file:

```C
#ifndef __ASSEMBLER__
/*
 * C code goes here...
 */
#else   /* __ASSEMBLER__ */
/*
 * Assembler code goes here...
 */
#endif  /* !__ASSEMBLER__ */
```

#### `.macro` and `.endm` assembler directives

`.macro` and `.endm` together define a pseudo-instruction, which is replaced
by the instructions inside `.macro` and `.endm` directives.

An example usage is the `cpuid` pseudo-instruction implemented in `<asm/smp.h>`
for getting CPU ID on MIPS32/64 Revision 2+ processors:

```
        .macro  cpuid result temp
        MFC0    \result, CP0_EBASE
        andi    \result, EBASE_CPUNUM_MASK      /* 0x3ff */
        .endm
```

Then, if one writes the pseudo-instruction

```
        cpuid   v0, v1
```

The preprocessor would replace the pseudo-instruction above with

```
        MFC0    v0, CP0_EBASE
        andi    v0, EBASE_CPUNUM_MASK           /* 0x3ff */
```

Then, according to `<asm/asm.h>`, the preprocessor would replace `MFC0` with
`mfc0` or `dmfc0`, according to whether we're targetting MIPS32 or MIPS64
processors.

### Obtaining CPU ID on MIPS Revision 1

The bad news is that, MIPS32/64 Revision 1 (which is the case of MSIM) didn't
specify how to obtain the CPU ID, as multiprocessor is not popular by the
time MIPS32/64 Revision 1 is proposed.

So, the method is entirely open to machine implementation.

MSIM defined a `dorder` device, whose documentation is inside the reference
manual.  In short story, one can obtain the processor number register by
reading the *processor number* register in the `dorder` device.

##### Programming exercise

1. Implement the `__cpuid` function to obtain the CPU ID in MSIM in
  `<mach/smp.h>`, which is originally `smp-mach.h`.
2. Declare and implement the `cpuid` pseudo-instruction for MSIM,
  in `<mach/smp.h>`, using the syntax you've just learned for defining a
  pseudo-instruction yourself.

### Suspending slave processors

As mentioned above, we should suspend the slave processors until the
master reports that the slave processors are allowed to continue.

The logic is quite simple as each processor has its own mailbox:

* The master processor puts the entry address for slave processors inside
  their mailboxes.
* The slave processors clear their mailbox at the beginning, then poll their
  mailboxes until their mailboxes receives an entry address.  After that, the
  slave processors jump to the address indicated in the mailbox.

As we're only allowing the master processor to initialize hardware,
load kernel, etc., we should suspend the slave processors as early as
possible.

The earliest code being executed is obviously the ones after `0xbfc00000`,
i.e. the `__reset` function in Lab 1.  So, we're going to modify the code
there.

Assuming that you've copied the sample code of `__reset` in Lab 1:

```
NESTED(__reset, 0, ra)          /* 0xbfc00000 */
        mfc0    a0, CP0_STATUS
        ori     a0, ST_ERL
        xori    a0, ST_ERL
        mtc0    a0, CP0_STATUS
        li      sp, 0x8f000000
        jal     main
END(__reset)
```

Obviously, we should clear the `ERL` bit, which indicates an error, for every
processor.  Therefore, we should suspend the slave processors after
clearing the `ERL` bit:

```
NESTED(__reset, 0, ra)          /* 0xbfc00000 */
        mfc0    a0, CP0_STATUS
        ori     a0, ST_ERL
        xori    a0, ST_ERL
        mtc0    a0, CP0_STATUS
        cpuid   a1, a2          /* Put CPU ID in register a1 */
        bnez    __slave_entry   /* Jump elsewhere if slave */
        li      sp, 0x8f000000
        jal     main
END(__reset)
```

The problem becomes how to implement `__slave_entry`.

Assuming that the physical address of `dorder` device is `MSIM_ORDER_BASE`,
and its virtual address being `MSIM_ORDER_BASE_VA`, we could write the following
code to clear the mailbox, then poll it until something comes in.

```
__slave_entry:
        li      a1, MSIM_ORDER_MAILBOX_BASE_VA
        sw      zero, (a1)
poll:   lw      a0, (a2)
        beqz    a0, poll
        jr      a0
```

#### Numerical labels in MIPS

The following code is identical to the `__slave_entry` code above:

```
__slave_entry:
        li      a1, MSIM_ORDER_MAILBOX_BASE_VA
        sw      zero, (a1)
1:      lw      a0, (a2)
        beqz    a0, 1b
        jr      a0
```

The `poll:` label is replaced with a number, which is called a *numerical
label*.

Also, the branching instruction target changes from `poll` to `1b`, which means
to branch to numerical label named `1` *backward*.

One can jump to a numerical label *forward* as well:

```
        /* Sample code snippet */
        lw      a0, (a2)
        beqz    a0, 1f          /* Jump to numerical label 1 forward */
        /* ... */
1:      addu    a0, a1
        /* ... */
```

The advantages for adopting numerical labels are:

1. You can have as many
  numerical labels named "1" as you like.  In case of multiple numerical
  labels with the same name, the branching instruction targets the nearest
  one backward or forward.
```
        /* Sample code snippet 2 */
1:      lw      a0, (a2)
        beqz    a0, 1b          /* Branch to nearest numerical 1 backward */
1:      lw      a1, 4(a2)
        blez    a1, 1b          /* Branch to nearest numerical 1 backward */
```
2. You don't need to invent various names for each label.

The disadvantage is that the code becomes less readable with numerical
labels.

##### Programming exercise A (optional)

Since our mailboxes are 16-bytes long each, you can put additional information
into the mailboxes, for example, the initial value of `sp`, the stack pointer.

Change the firmware assembly code to read the initial `sp` from the mailbox
for each slave.

## SMP startup: kernel

We now know that all slaves would wait for the master to prepare environment
for their execution.

After the master prepares everything, it should call a C routine
`smp_startup()`, which puts the entry addresses for each slave, as well as
some other arguments (if any), into their mailboxes.

The slaves should do some CPU state initialization during kernel entry,
and jump to `slave_main()` function.

The logic of `slave_main()` is simple:

```C
void slave_main(void)
{
        kprintf("Hello from CPU #%d!\n", cpuid());
        for (;;)
                /* nothing */;
}
```

##### Programming Exercise

1. Add the logic of `slave_main()` to your codebase.
2. Design and implement the slave's entry points.  Initialize the CPU like
  the master, and jump to `slave_main()` afterwards.
    - You can write a separate entry point dedicated to slaves, just like in
      xv6, or you can rewrite and reuse the main entry point `__start`.
3. Implement `smp_startup()` function, where the master puts each slave's entry
  points into its mailbox, as well as other arguments.

Think of where the new functions or entries should be put, and whether the logic
of new functions or subroutines are machine-specific, architecture-specific, or
generic.

Create new source files or headers as needed.

##### Programming Exercise

Recall that we should have declared a buffer for kernel stack in Lab 1:

```C
extern unsigned char cpu_stack[];
```

1. Redefine the buffer as a two-dimensional array, with CPU ID as
  the first dimension.
2. Modify `smp_startup()` or the slaves' entry points to initialize the
  stack pointer register `sp`, depending on whether you have completed
  **Programming Exercise A**.
3. Modify the Makefile of kernel accordingly, then build and run your project,
  to see if it works as intended.  If it's printing garbage, try to infer
  the reason.

## Synchronization basics: locks

The reason why your program above didn't work as expected is that we didn't
control the access to the line printer.  When we're calling `kputs()`, which
is the underlying call of `kprintf()`, the processor making such call should
gain *exclusive* access to the line printer.  However, no such mechanism
is developed in your program.

From then on, we're going to introduce *locks*, which is the simplest tool
for ensuring exclusive access to a resource.

A lock maintains two states: *locked* and *unlocked*.  It also provides two
primitives:

1. `release()`, or `unlock()`:
    - If the state is locked, set the state to unlocked.
2. `acquire()`, or `lock()`:
    - Wait until the lock is unlocked, then change its state to locked, all
      completed **at the same time, uninterrupted and exclusive**.

The emphasized part, which is a kind of *atomicity*, a broader concept, is
especially important,
since two processors detecting an unlocked lock simultaneously, and change the
state to locked simultaneously, would result in two processors "exclusively"
controlling the same resource, rendering the lock mechanism useless.

### Read-Modify-Write operation

Most processors provide atomic or exclusive operations to access a part of
memory.  *Read-Modify-Write* (RMW) operation provides one scenario, where
a read followed by a modification, then a write, is done atomically.

On x86, the `lock cmpxchg` instruction implementing a primitive called
*compare-and-swap*, an alternative to RMW operation.

On ARM and MIPS, RMW operations are implemented as a mechanism called
*Load-Link/Store-Conditional*.

### `ll` and `sc` instruction

Here's the short story about the purpose of `ll` and `sc`.

Load-link instruction `ll` loads from memory into a register.

Store-conditional instruction `sc` writes from register into the memory
only if the memory content there didn't change since the last `ll`
operation.  The register changes to 1 if `sc` successfully writes
into memory, or 0 otherwise.

The precise behavior is described in MIPS64 Architecture Volume II.

#### A concrete example

Consider the scenario where a global counter is maintained.  The
following code increases that global counter:

```
        /* Sample code snippet 3 */
        lw      a0, (a1)        # (1a)
        addu    a0, 1           # (2a)
        sw      a0, (a1)        # (3a)
```

However, when concurrency or parallelism takes place, the code snippet
above would become flawed.

Suppose that two processors are executing the code snippet at the
same time, with the same `a1` value, pointing to the same global counter.
The code won't work as intended because we intended to increase the counter
twice, while the actual effect is that the counter is only increased by 1.

This is where `ll` and `sc` kicks in.  Atomicity is guaranteed by modifying
the code above to

```
        /* Sample code snippet 4 */
1:      ll      a0, (a1)        # (1b)
        addu    a0, 1           # (2b)
        sc      a0, (a1)        # (3b)
        beqz    a0, 1b          # (4b)
```

As `sc` modifies the global counter *only if* the content there didn't change
since the last `ll` performed by the processor, and the whole process
would be repeated if `sc` fails to make changes, the global counter
is ensured to increase by one each time the snippet is executed, regardless
of concurrency and parallelism.

### Encapsulating into C functions

We need to encapsulate the code for such RMW operations into C functions
because

1. The developer shouldn't care about the implementation detail when using
  it.
2. The implementation detail is architecture-specific.
3. Writing assemblies everywhere is painful.

The encapsulated functions are already given in `<asm/atomic.h>`.

### `<asm/atomic.h>`

The primitives provided by `<asm/atomic.h>` are:

- `atomic32_get_bit`: Atomically get the n-th bit of an integer
    + It's identical to normal get bit, though.
- `atomic32_set_bit`
- `atomic32_change_bit`
- `atomic32_clear_bit`
    + The three functions above repeatedly tries to set/change/clear the bit
      atomically until success.
- `atomic32_try_set_bit`
- `atomic32_try_change_bit`
- `atomic32_try_clear_bit`
    + The three functions above tries to set/change/clear the bit atomically
      and may fail by returning `false`.

Most of these functions are written in *inline assembly*, a powerful GCC
feature to allow embedding assembly code into C code, enabling developers
to write an architecture-specific function mostly in C, while implementing
core parts inside a function in assembly.

We'll go through one function, `atomic32_set_bit`, to go through the syntax of
GCC inline assembly, since the rest implementations are very similar.

Here are detailed documentations of
[GCC Extended Assembly](https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Extended-Asm.html)
and
[Inline Assembly Constraints](https://gcc.gnu.org/onlinedocs/gcc-4.9.3/gcc/Constraints.html).

#### A walkthrough of `atomic32_set_bit`

The function body of `atomic32_set_bit` is

```C
static inline void atomic32_set_bit(volatile uint32_t *addr, int pos)
{
        uint32_t result;
        asm volatile (
                "1:     ll      %[reg], %[mem];"
                "       or      %[reg], %[val];"
                "       sc      %[reg], %[mem];"
                "       beqz    %[reg], 1b;"
                : [reg]"=&r"(result), [mem]"+m"(*addr)
                : [val]"ir"(1 << pos)
        );
}
```

- The `asm` keyword starts the inline assembly.  The C compiler would embed
  the assembly code block inside the parentheses into the C function.
- The `volatile` keyword following the `asm` keyword instructs the compiler
  *not* to move the assembly code block around for optimization.
    + One should be familiar with `volatile` in Lab 1.
- The C compiler would automatically join the string literals into one if
  they're next to each other.
- `ll`, `or`, `sc` and `beqz` are MIPS instructions.\
- The operands `%[reg]`, `%[mem]`, and `%[val]` would be replaced by
  register operands, memory operands, or immediate operands (which are
  constants), according to the list given below.
- The comma-separated list succeeding the first colon is called
  the *output variable list*, where the instruction puts the result.
- The comma-separated list succeeding the second colon is called
  the *input variable list*, where the instruction receives input.

The syntax of variable list is

```
[<operand-name>]<constraint-string>(<variable>)
```

For example, the operand `%[reg]` would be replaced into a register
operand, or a memory operand, pointing to the variable `result`, in the
code above.

*Constraints* indicate which kinds of operand the compiler is allowed to
translate the variable into.

- `r` allows translation into register operand.
- `m` allows translation into memory operand.
- `i` allows translation into immediate operand (where the variable is
  a constant)
- `=` indicates that the content in the variable is write-only.
- `&` tells that the variable would be changed *before* input variables
  are used.  This essentially prevents the compiler to translate the
  variable into memory operand or register operand for input.
- `+` indicates that the operand is both read and written.

##### Programming exercise (deferrable)

Try to implement the `atomic_add` and `atomic_try_add` primitives.

##### Programming exercise (deferrable)

Implement a more general `get_bit`, `set_bit`, `change_bit` and `clear_bit`
primitive, and the "try" equivalents of the latter three, in `<asm/atomic.h>`.
The primitives get/set/change/clear the `pos`-th bit starting from byte address
`addr`.

Note that `pos` can be arbitrary large as long as `unsigned int` permits.

The function signature of, say, `atomic_get_bit` is

```C
static inline bool atomic_get_bit(volatile void *addr, unsigned int pos);
```

You don't need to write inline assembly yourself; just call the already-exist
32-bit primitives for performing the actual atomic operation.

## Synchronization basics: spinlocks

*Spinlock* is the simplest lock.  The processor simply polls the lock state
until the lock is released.

The necessary data structure of spinlock is extremely simple:

```C
/* spinlock.h */

/* ... */
struct spinlock {
        uint32_t        state;
#define LOCK_BIT        0
};
/* ... */
```

##### Programming exercise

Implement the spinlock logic.

1. Think of the design decision: shall we implement the `lock()` and `unlock()`
  functions as "member functions" of a common `lock` abstract class?  Give the
  reason for your choice.
    - It's totally OK if your answer is "by intuition" or "by chance" etc.
2. (Optional) modify the `spinlock` structure to hold additional information,
  e.g. a structure reference holding the information of the locking CPU:
```C
struct cpuinfo {
        uint32_t cpuid;
        /* ... */
};
```
3. Implement the spinlock primitives `spinlock_lock()` and `spinlock_unlock()`,
  using the primitives given, as an attempt of abstracting the
  architecture-independent spinlock logic.
    - Oddly, Linux implements spinlocks in architecture-specific code.  You'll
      see the reason soon, though.
4. Review your project, and make sure that every logic, declaration,
  implementation, etc. are in *their* place.

##### Programming exercise

Add a spinlock in your `kputs()` function, to ensure that the processor gain
exclusive access to the line printer.

Change your Makefile and build your project.

Your code still doesn't work, LOL.  Try to figure out the reason yourself.

### A trap in locking

Your code for locking above would probably look like:

```C
void spinlock_lock(struct spinlock *spinlock)
{
        do {
                while (atomic_get_bit(&spinlock->state, LOCK_BIT))
                        /* nothing */;
        } while (atomic_try_set_bit(&spinlock->state, LOCK_BIT));
}
```

However, obviously the locking procedure could be interrupted between waiting
the lock state and changing the state.

Suppose that CPU #1 and CPU #2 both observed that the state is unlocked.  They
jump out of the inner loop.  If CPU #1 initiates and completes the `try_set_bit`
primitive first, then CPU #2 initiates and completes the logic next, both
of them may succeed, thinking that they've obtained the lock.

Therefore, the locking logic should be implemented to be atomic as a whole,
which probably requires you to write an inline assembly.

##### Programming exercise

Change your spinlock logic and move them to architecture-specific directory.

You can refer to the code in `<asm/atomic.h>` to see how to write inline
assembly.  The code there should be similar to what you'll write.

Your code should work then.

