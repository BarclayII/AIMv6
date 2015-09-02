# Lab 0: Prerequisites (MIPS / Loongson3A)

This document aims to provide an overview and everything necessary to enable
the users to do MIPS experiments.

### What's MIPS?

MIPS is originally an acronym of *Microprocessor without Interlocked Pipeline
Stages*.

MIPS is a super-simple architecture:
* All instructions are fixed-length.
* All integer instructions are simple.
  + Arithmetic can only take place among registers and immediate values.
  + Memory can only be accessed by loads and stores.
* All floating-point instructions are simple (but we don't need them in
  our experiments).
* Processor controlling is simple.
  + Processors are controlled by coprocessors, accessible via instructions
    like `mfc0` and `mtc0`.  Processor modes, interrupts, etc., are all
    controlled in coprocessors.
* Exception handling is simple.
* Memory management is simple (as in hardware).
  + The memory management unit (MMU) in MIPS *only* consists of a TLB.
    Failing to convert virtual address into physical address simply results in
    an exception to be handled by software.
  + Furthermore, MIPS predefined a set of rules for mapping virtual addresses
    to physical ones.
* Cache management is simple (as in hardware).
  + Early MIPS leaves cache management to software by providing a `cache`
    instruction.  It's lucky for us that Loongson maintains cache coherency
    by hardware, so we can treat the caches as transparent in most cases
    (but see cache aliasing mentioned below).
* ...

### References

MIPS Architecture for Programmers

1. [**Volume 1**](http://www.weblearn.hs-bremen.de/risse/RST/docs/MIPS/MD00083-2B-MIPS64INT-AFP-00.95.pdf)
2. [**Volume 2**](http://scc.ustc.edu.cn/zlsc/lxwycj/200910/W020100308600769158777.pdf)
3. [**Volume 3**](http://www.weblearn.hs-bremen.de/risse/RST/docs/MIPS/MD00091-2B-MIPS64PRA-AFP-00.95.pdf)

### Preface

Before moving on to explore the mechanism of MIPS, it's highly recommended
to preview the instruction set in **Volume 2**, as well as
[this Wikipedia article](https://en.wikipedia.org/wiki/MIPS_instruction_set),
to have a rough impression of what MIPS is able to do.  There's no need to
fully understand the whole instruction set at present, as we'll review the
purpose of some instructions from time to time while explaining the underlying
ideas.

### Pipeline

Executing an instruction involves decoding the binary value into operation and
operands, performing such operation, accessing memory if needed, and reading or
writing registers.

Processing an instruction needs some time due to latencies such as gate
latency.  The simplest solution for sequentially processing the instruction
is to install a clock, and make the processor to fully execute one instruction
per cycle.  That is, the processor decodes the binary value, performs
arithmetic, reads or writes memory... *in a single cycle*.  Such cycle could
be quite long, as it has to include all stages of execution.  Moreover, while
one stage component is working, all others are idle.

The idea employed by MIPS of reducing cycle, and thus increasing frequency,
is to process all the stages *in parallel*:

1. Each stage works on a separate instruction simultaneously.
2. The stage components are synchronized by a clock, i.e. the clock cycle
  is now the (maximum) duration of processing a single stage.

This *pipelining* technique fully utilizes all components, thus improves
performance by increasing throughput, i.e. by increasing the number of
processing instructions per unit time.

##### Classic RISC pipeline

The process of instruction execution can be generally split into five
stages which take the same amount of time:

1. *Fetching* instruction from memory (**F stage**),
2. *Decoding* the binary value and finding the operation as well as the
  operands, including register reading (**D stage**),
3. *Executing* arithmetics (**E stage**),
4. *Memory* accessing (**M stage**),
5. *Writing Back* to registers (**W stage**).

Readers of *Computer Systems: A Programmer's Perspective* should be familiar
with these five stages.

MIPS adopts this pipeline, the only difference being that D stage and W stage
are designed to take only half a cycle.

The timing diagram is displayed below:

```
1---*---2---*---3---*---4---*---5---*---6---*---7
|   F   | D |   E   |   M   | W |
+-------+---+---+---+-------+---+---+---+
        |   F   | D |   E   |   M   | W |
        +-------+---+---+---+-------+---+---+---+
                |   F   | D |   E   |   M   | W |
                +-------+---+-------+-------+---+
```

##### Hazards

*Hazards* are situations where the execution of a pipeline yields results
different than ordinary sequential execution.  By introducing redirections
the processor can solve most kinds of hazard except:

1. **Branch hazard**:
  In conditional branches, whether to branch to target or not is determined
  in the midpoint of E stage, or at the start of fetching the second
  instruction after the current one.  That is, the instruction directly
  succeeding the conditional branch is *already fetched*.
2. **Load hazard**:
  From the pipeline above we can induce that a loading instruction puts data
  into register *after* the arithmetic of next instruction takes place, which
  may result in inconsistent results.

Usually, the processor automatically inserts bubbles, renames registers or
use other complex techniques to address these problems, but MIPS takes a
different approach by explicitly specifies *branch delays* and *load delays*.

###### Branch delay slots

*Branch delay slots* are instructions directly succeeding branches.  They're
*always* executed, regardless of whether the branch really takes place or
not.  For example, suppose `r2 = 3, r3 = 4, r4 = 7`, where `r2`, `r3`, `r4`
stands for different registers.  Consider the following instruction sequence

    i1:     bne   r2, r3, i4
    i2:     addiu r2, r2, 1        # branch delay slot
    i3:     addu  r3, r3, r2
    i4:     subu  r4, r3, r2

The processor first fetches i1, determines that the condition (not equal)
is satisfied, then executes the one inside the branch delay slot, increasing
`r2` by one, and finally jumps to `i4` and subtracts `r3` and `r2`, storing
the result (0) into `r4`.

GNU assembler automatically rearranges instructions or insert `NOP`s so that
one can write assemblies as if no delays take place, unless `.set
noreorder` directive is specified (see **Common assembler directives** section).

###### Load delays

Early MIPS processors introduces load delay slots to handle load hazards,
but most modern processors, including Loongson, prefers suspending the
processor or use other techniques to resolve load hazards by hardware.
Also, GNU assembler can rearrange instructions to avoid load hazards.

### Common GNU assembler directives

*Assembler directives* are not instructions.  They only directs the assembler's
behavior.

Here is a list of common GNU assembler directives:

1. `.set push`: Save current assembler settings on the assembler setting stack.
2. `.set pop`: Restore assembler settings from the top of assembler setting
  stack.
3. `.set noreorder`: Disable reordering of instructions, preventing the
  assembler from optimization and automatic resolution branch hazards: one
  should take branch delays into consideration when writing assemblies then.
4. `.set reorder`: Enables reordering, allowing optimization within specified
  level, and rearranging instructions, possibly inserting `NOP`s, to deal with
  branch delays.  This is enabled by default.
5. `.set noat`: Does not allow reservation of `$1` as *assembler temporary
  register* (`at`).  See **Register convention** for details.
6. `.set at`: Reserve `$1` for assembler use only, and user programs should
  not use register `$1`.  See **Register convention** for details.  This is
  enabled by default.

### `<asm/asm.h>`

The header file `<asm/asm.h>` (in `include/arch/mips64/asm/asm.h`) defines
macros for defining subroutines and global symbols.

1. `LEAF(symbol)`: Defines the start of a *leaf function*, that is, a
  function with makes no further function calls.
2. `NESTED(symbol, stack, retreg)`: Defines the start of a function which
  makes other function calls.  `stack` indicates the stack size for this
  function.  `retreg` is the register to be saved, which is usually
  the return address register `$31`, or `$29` for exception handlers.
3. `END(symbol)`: Marks the end of an assembly function.

### Application Binary Interface

From now on I'll introduce how assembly programs interact with C programs,
which is called *application binary interface* (ABI).
We don't want to write everything in assembly, after all.

MIPS has two common ABIs, one for 32-bit processors, the other for
64-bit ones.  For GCC, ABI could be specified by giving `-mabi` flag.

##### Register conventions

###### *Temporaries* and *statics*

In almost all architectures the specifications somewhat designates a subset
of registers whose values are to be preserved (that is, saved to memory)
between function calls.  These registers are called *statics*, or
*callee-saved registers*.  The callee (that is, the function being called)
saves these registers into memory prior to entrance, and restores those
registers after exiting.

Other registers are called *temporaries*, or *caller-saved registers*.
The values there need not be preserved between function calls, and therefore
it's the responsibility of the caller (the function making calls) to
preserve those registers.

###### List of conventions

MIPS processor introduces 32 general purpose registers (GPR).  Their
conventions are listed below.

1. `$0` (`zero`): **Hardwired zero**.  The value of this register is always
  zero.
2. `$1` (`at`): **Assembler temporary**.  This register is reserved for
  assembler's use for performing arithmetic with large constants when
  `.set at` is turned on.
3. `$2` (`v0`): Stores return value of the function.
4. `$3` (`v1`): Also stores return value of the function, although in
  practice the compiler rarely does so.  Sometimes in 32-bit ABI
  `v1` stores the high-order 32 bits for `long long` values.
5. `$4` to `$7` (`a0` to `a3`): Stores the values of first four arguments
  function.  Passing structures as arguments complicates things, but
  passing pointers is a better programming practice anyway, as it
  avoids unnecessary duplication of structures.
6. `$8` to `$11`: The purposes are different between 32-bit and 64-bit ABI:
  - `t0` to `t3`: Temporaries in 32-bit ABI.
  - `a4` to `a7`: Stores the next four arguments of a function in 64-bit ABI.
7. `$12` to `$15` (`t4` to `t7`): Temporaries.
8. `$16` to `$23` (`s0` to `s7`): Statics.
9. `$24` (`t8`): Temporary.
10. `$25` (`t9`): Temporary, although it has other use in *position-independent
  code* (PIC).
11. `$26` and `$27` (`k0` and `k1`): Reserved for exception handlers, and
  should NOT be used by anything else.
12. `$28` (`gp`): *Global pointer*.
13. `$29` (`sp`): *Stack pointer*, pointing to stack top.
14. `$30` (`fp` or `s8`): *Frame pointer* in most cases for indicating the
  stack bottom of current stack frame.  Rarely it's used as yet another
  static.
15. `$31` (`ra`): *Return address register*.  The hardware stores the return
  address after execution of a branch-and-link.  Functions issue a `jr ra`
  to exit itself.

Only `s0` to `s7`, `sp`, `fp`, `ra` (and sometimes `gp`) are statics.  All
other registers are temporaries.

##### Data types

|C type          |Assembler type|Size (bytes)|
|:---------------|:-------------|:-----------|
|`char`          |`byte`        |1           |
|`short`         |`half`        |2           |
|`int`           |`word`        |4           |
|`long long`     |`dword`       |8           |
|`float`         |`word`        |4           |
|`double`        |`dword`       |8           |

In 32-bit ABI `long` is equal to `int`, and in 64-bit ABI `long` is the same
as `long long`.

##### Endianness

Big-endian puts high-order bits at lower address, while little-endian puts
low-order ones at lower address.  For a word (`int`), the first byte stores
bit 0..7 in little-endian processors, or 31..24 in big-endian ones.

Loongson 3A is little-endian.

##### Memory layout of structures

Complex types (structures) are organized in memory by concatenating each
primitive data type sequentially, inserting padding bytes to respect
alignment rules (e.g. `short`s start on 2-byte boundaries, while
`int`s start on 4-byte ones).

An example is provided below:

```C
struct {
	char	c;		/* byte 0 */
	/* 1 byte padding */	/* byte 1 */
	short	s;		/* byte 2-3 */
	int	i;		/* byte 4-7*/
	short	t;		/* byte 8-9 */
	/* 6 bytes padding */	/* byte 10-15 */
	long long ll;		/* byte 16-23 */
};
```

`packed` attribute in GNU C breaks alignment rules and does not introduce
any padding.  Make sure that the compiler generates correct code!  Old
versions of GCC may be buggy in this matter.

`aligned` attribute, in reverse, forces the compiler to arrange members
to align to bytes specified.

`#pragma pack` is a replacement of GNU-specific `packed` and `aligned`
attributes.

##### Stack

The stack in MIPS convention grows downward, with stack top indicated by
register `sp` (`$29`).

##### Argument passing

###### Primitive argument passing

In this section it's assumed that all arguments are of primitive data types
(i.e. basic data types, and pointers).

There are two ways of passing arguments, determined by number of arguments
required:

1. When the number of arguments is no more than 4 (8 in 64-bit), the values
  are simply put inside argument registers `a0` to `a3` (`a0` to `a7` in
  64-bit) in that order.
2. When the number of arguments is more than 4 (8 in 64-bit), the caller
  allocates from the stack an equal number of 4-bit (8-bit in 64-bit)
  *argument slots*.  The
  first 4 (8 in 64-bit) slots from the lowest address are reserved, and the
  slots thereafter contain the fifth (ninth in 64-bit) and more arguments
  respectively.  The first 4 (8 in 64-bit) arguments are still placed in
  `a0` to `a3` (`a0` to `a7` in 64-bit).

Things become complicated when passing 64-bit arguments in registers in 32-bit
ABI.  Usually each of these arguments is unpacked into 2 32-bit values, with
high-order bits at higher argument register in little-endian (lower one if
big-endian).  If the first argument does not occupy two registers, it is
sign-extended to fit into two registers.

###### Passing structures

When someone does intend to pass a structure as argument.  The argument slots
and registers are filled by the content of structure.  Therefore the number
of allocated slots may grow, and may cause troubles when making calls to or
from functions written in assembly.

Therefore, although allowed and supported by C, directly passing structures is
NOT recommended.  In most cases passing pointers of structures would suffice,
since it's easier to write assemblies for interaction, and furthermore, takes
less time and space by avoiding unnecessary duplication defined by C semantics.
Use a `const` pointer for read-only access of a structure.

##### How does a function grow its stack frame

The assembler precalculates the space it needs to store local variables, and
to hold statics and arguments before making function calls.
