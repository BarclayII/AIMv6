# Lab 6 - A brief walkthrough

## Overview

1. Create the very first user process.
2. Run a piece of code in user space.
3. Implement the following debug system calls, which would be removed in future:
    - `__tokernel` for switching back to kernel code.
    - `__puts` for debug printing.
4. Implement a preliminary C runtime.

## Creating the first process

A process is usually assigned a *process control block* (PCB).

As you have already implemented `kmalloc()` and `kfree()` functions,
you can use them to conveniently allocate and deallocate PCBs.

A PCB usually contains:

1. A *process ID* (PID).
2. A reference to parent process.
3. A structure containing current context (when doing context switches).
4. A reference to top-level page table.
5. A separate kernel stack for handling system calls.

Refer to xv6/uCore code for more information you want to store in PCBs.

## Context switch

Context switch could be categorized into:

1. Switching between user space and kernel space, and
2. Switching from one process to another.

### Between user and kernel

The first kind could be easily implemented by exception handling: we just
simply set the `ST_KXL` bitfield to `KSU_USER` (both defined in
`asm/cp0regdef.h`) in trap frame.  The processor mode would then automatically
become user mode after returning from exception handler.

To switch from user mode into kernel mode, one could simply cause an exception.
However, such switches should be strictly constrained, as one of the principles
of the operating system is that

1. Kernel is always trustworthy, and
2. Users are never trustworthy.

Other than interrupts and exceptions, where specific, dedicated routines are
being executed, the most direct, controlled way to switch from user mode
to kernel mode is via a *system call*, along which a number is passed to
indicate which service the user program is requesting from the kernel.

### Between processes

Since we are designing a time-sharing system, the kernel should schedule
processes, i.e. suspend the current process and wake up another one, from
time to time.  The time between such scheduling is usually determined by
timer interrupts.

Apparently, a process should be able to continue its execution correctly
after context switches.

As context switches happen during timer interrupts, which are exceptions,
the context switch routine happens in an exception handler.  We can draw
a flow diagram from the view of a process as follows:

```
         [user program]         
               |
               |
               |
               v
    [exception handler entry]   
               |
               |
               |
               v
 [timer interrupt handler entry]
               |
               |
               |
               v
       [scheduler entry]        
               |
               |
               |
               v
     [context switch entry]     
               |
               |
               |
               v
      [context switch exit]     
               |
               |
               |
               v
        [scheduler exit]        
               |
               |
               |
               v
  [timer interrupt handler exit]
               |
               |
               |
               v
     [exception handler exit]   
               |
               |
               |
               v
     [user program again]       
```

However, from the view of kernel, the flow diagram becomes:

```
       [user program A]                 [user program B]         
               |                                .
               |                                .
               |                                .
               v                                v
    [exception handler entry]        [exception handler entry]   
               |                                .
               |                                .
               |                                .
               v                                v
 [timer interrupt handler entry]  [timer interrupt handler entry]
               |                                .
               |                                .
               |                                .
               v                                v
       [scheduler entry]                [scheduler entry]        
               |                                .
               |                                .
               |                                .
               v                                v
     [context switch entry]           [context switch entry]     
               \                                .
               .------------------------------- .
               .                               \.
               v                                v
      [context switch exit]           [context switch exit]     
               .                                |
               .                                |
               .                                |
               v                                v
        [scheduler exit]                [scheduler exit]        
               .                                |                             
               .                                |
               .                                |
               v                                v
  [timer interrupt handler exit]  [timer interrupt handler exit]
               .                                |
               .                                |
               .                                |
               v                                v
     [exception handler exit]        [exception handler exit]   
               .                                |
               .                                |
               .                                |
               v                                v
     [user program again]            [user program again]       
```

## C runtime

Usually, we compile a user program by directly writing the `main` function
in C code:

```C
int main(int argc, char *argv[])
{
        /* blah blah blah */
}
```

However, the entry of the program is not the `main` function, as you can see
from utilities such as `readelf(1)` or `objdump(1)`.

Actually, the C compiler often wraps your user program with a small C runtime
code, containing the "real" entry usually named `__start`.  After returning
from `main()`, the control is transferred back to C runtime, in which the
`exit(2)` system call is invoked, terminating the process.

### MIPS ABI convention: return values

The code wrapping the user C program could be either written in C or in
assembly.  If one wants to write an assembly wrapper (as the wrapper is often
quite small), remember that MIPS ABI requires the return values to be stored
in register `v0` (and `v1`, in case where one register is not enough).

For example, the following function

```C
int add(int a, int b)
{
        return a + b;
}
```

Would be translated to something like

```
# a0: int a
# a1: int b
# v0: return register
# Not considering branch delay slots
addu    v0, a0, a1
jr      ra
```

### Linux/MIPS system call convention

This section is only for reference.  You can design your own system call
convention if you like.

1. System call number is placed in register `v0`.
2. Arguments are placed as if one is calling a function.
    - When the number of arguments is lower than 4, registers `a0` to `a3`
      store those arguments.
    - One can circumvent the argument number limit by passing structures
      to combine some of the arguments into one single argument.  This is
      usually done by an adapter in your C library.  Thus, you can maintain
      POSIX compliance whilst interacting with your own system call
      convention.
3. After system call, the error number `errno(3)` is stored in `a3`
   register.
4. After system call, the return value is stored in `v0` register.
    - `pipe(2)` is a notable exception on some operating systems as `v0`
      and `v1` are both employed to
      store the two file descriptors.  Putting those descriptors into
      the buffer you provided is again done by the C library to maintain
      POSIX compliance.

#### An example

Suppose that one is making an `exit(2)` system call:

```C
exit(1);
```

The `exit` function is actually a function in C library, which extracts
and arranges the arguments and invoke the system call.  As the argument
is already there (`a0` already contains the needed exit code 1 as per
MIPS ABI), and `v0` could be directly used as a return register, the
assembly code simply looks like:

```
LEAF(exit)
        li      v0, NRSYS_exit          # the system call number of exit
        syscall
        jr      ra
END(exit)
```

## Building a binary into the program

We are going to create the first user process whose logic is in `init.c`
given in this lab.  A convenient way is to build the code into a program,
bundle the program with the kernel, and let the kernel spawn a process
with the embedded program.

After compiling and linking the program code with other necessary library
codes, including C runtime, C library functions.  The only thing
remaining is to bundle the program with the kernel, which could be
accomplished by an `ld` option:

```
$(LD) $(LDFLAGS) -o $(BINARY) -b binary <path-to-your-user-space-program>
```

The user space program is then embedded into the kernel, with the following
symbols added:

1. `_binary_<TRANSDUCED_PATH>_start`: the beginning
  of your embedded program.
2. `_binary_<TRANSDUCED_PATH>_end`: the end of your embedded program.
3. `_binary_<TRANSDUCED_PATH>_size`: the size of your embedded program.

where `<TRANSDUCED_PATH>` is the path to your user program file, with
every punctuations (slash, comma, hyphen, etc.) substituted with underscore
(`_`).

##### Programming task

1. Implement the C runtime code wrapping the user `main` function.
2. Implement the C library functions wrapping `__tokernel` and `__puts` system
  calls, where
    - `__tokernel` is to switch back to kernel context.
    - `__puts` is to output a string.  More precisely, it is a user interface to
      `kputs`.
3. Build your user space program.
4. Implement the code for creating the first user process.  Refer to xv6 code
  for how to correctly setup the trap frame.  Also, refer to `switch.S` for
  how a context switch happens exactly.
5. Implement the `__puts` system call and the `__tokernel` system call.
6. Switch context to the user process.  Refer to the diagram above for a
  possible workflow.
    - The context switch routine, `switch_context()`, is already provided.
      Read `switch.S` to figure out how the process context structure should
      look like, and what are the purposes of each argument.
7. Run your code.

Again this is a very heavy task, feel free to ask questions here and there.
