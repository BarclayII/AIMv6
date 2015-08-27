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

