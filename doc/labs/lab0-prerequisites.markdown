# Lab 0: Prerequisites

Designing an operating system certainly involves an *awful* amount of work.

Before you take this experiments, you must prepare for:
1. Reading a LOT of documentations, specifications, technical manuals, and
   anything else you can or can't imagine.  Some of them are detailed and
   self-evident, while others are poorly-written.  We will provide as much
   resource as we can.
2. Reading and struggling to understand a LOT of code.  We are
   ~~evil~~ kind enough to provide a skeleton code for you to work with.
3. Doing a LOT of (re)searches, and asking MANY questions.
4. Doing, and *failing* a LOT of experiments.
5. (for those unluckies working on ARM/MIPS) Working WITHOUT A DEBUGGER.

As a side note: teamwork absolutely helps.

You may encounter mistakes, or bugs in our code or documentations as you move
on.  *Do not hesitate to report to us*.  Reporting bugs could be in any form,
from GitHub issues to emails.

If you're new to Unix-like systems, you could ramp up by checking out
the documentations in `unix` folder.  The documentations there may provide
a starting point.

Take a deep breath, and get yourself ready for your very first documentation.

### Setup

To do your operating system experiments you need the following:
1. A working Linux.
  * BSD or other Unix-like systems are OK if you can install GNU Compiler
    Collection (GCC).
2. A cross-compiler suite.
  * Currently GNU Compiler Collection (GCC) is the only compiler suite
    we're supporting.  Preferably, it should be the latest version, but
    our code supports all toolchains mentioned in the root README document.
  * If you insist on using other compiler suites such as
    CLang/LLVM, you're welcome to contribute to our repository.
3. A decent editor.
  * `vim(1)`, `emacs(1)`, `nano(1)`, ~~`ed(1)`~~ are all good choices.  You
    can also use IDEs as you wish.
4. Other tools, software and hardware.
  * ARM developers: make sure you have: (TODO: add hardware and software
    requirements)
  * MIPS developers: make sure you have:
    + A serial-to-USB converter.
    + A SATA-to-USB converter.

### Ramping up the ISAs

Ramping up the ISAs involves heavy reading.  Prepare yourself to read long,
boring, potentially poorly-documented manuals.

Here are some warmups, however:

For ARM developers: move to (TODO: specify documentation location)

For MIPS developers:
* There's an excellent book named *See MIPS Run* which provides an in-depth
  tour to the MIPS architecture, the way MIPS assemblies interact with C
  programs, and the way Linux works on MIPS.  We didn't include the book
  in our repository to avoid copyright issues, but you can probably find
  the electronic version on Internet.
* Or you can get a first impression in `mips64-loongson3a/prereq.markdown`.

