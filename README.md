# xxv6

Reimplementation and extension of teaching operating system xv6.

Architectures (to be) included: ARM, MIPS64, i386 (the original xv6 implementation)

Our first goal is to port xv6 onto ARM and MIPS64 (Loongson 2H).

The guides for placing and organizing source codes are in FILES in each directory.

Remember to add license information on top of the source code :)

This repository supersedes xv6-Loongson3A, the code from which will be migrated
there.

Please refer to the `doc` directory for documentations.

### Toolchain
Ideally, the project should be compilable by the latest GCC (or cross-GCC if
cross-compiling) toolchain available on either Linux distro.

If prebuilt packages are not available, it is always welcomed to use precompiled
binaries and libraries provided by the manufacturers.

If you're a determined, patient geek, you can build your own cross-compiler
from source.

#### Fedora
Fedora ships cross-compiler packages since release 18, and there's really no
reason to keep using unsupported old versions.

To install MIPS toolchain, execute (replace `dnf` with `yum` if you're running
older release)
```
# dnf install gcc-mips64-linux-gnu
```

Installing ARM toolchain is similar:
```
# dnf install gcc-arm-linux-gnu
```

#### Debian (**not tested**)
Cross-compilers are provided in *sid*, or *unstable* branch in Debian.

Unfortunately, one should usually upgrade to *unstable* from *testing*, which,
in turn, should be upgraded from *stable*.

If you don't want to risk your machine being "unstable" (though not unstable as
it seems - Ubuntu is based on Debian *unstable* branch), or regard the whole
process as troublesome, simply jump to the **Third-party** section.

Upgrading between branches is simply done by modifying `/etc/apt/sources.list`
and changing sources from *testing* to *unstable*.

After upgrading, the toolchains could be installed by
```
$ sudo apt-get install gcc-4.9-arm-linux-gnueabi
```
if you're playing with ARM board, or
```
$ sudo apt-get install gcc-4.9-mipsel-linux-gnu
```
if you're hacking a Loongson box.

Replace 4.9 with 5 if you want to try GCC 5.

#### Ubuntu (**not tested**)
Awkwardly, Ubuntu ships `gcc-4.9-arm-linux-gnueabi`, but does not ship the
MIPS equivalent.

For ARM developers, running
```
$ sudo apt-get install gcc-4.9-arm-linux-gnueabi
```
would do everything.

Obtaining MIPS toolchain is even more complicated.  First, you need to install
`gdebi`:
```
$ sudo apt-get install gdebi
```
Download and install by `gdebi` the following `.deb` packages from Debian *sid* 
repository, in the order given:  
[binutils-mipsel-linux-gnu](https://packages.debian.org/sid/binutils-mipsel-linux-gnu)  
[cpp-4.9-mipsel-linux-gnu](https://packages.debian.org/sid/cpp-4.9-mipsel-linux-gnu)  
[gcc-4.9-mipsel-linux-gnu](https://packages.debian.org/sid/gcc-4.9-mipsel-linux-gnu)  

#### Third-party
Loongson provided the compiler suite for building kernel [here](http://www.loongnix.org/dev/ftp/toolchain/gcc/release/CROSS_COMPILE/loongson3-gcc4.4.tar.gz)
However, Loongson only supports GCC 4.4.0, which is quite obsolete compared to
the now-popular version 4.9.x or 5.x.x.

Another good choice is Sourcery Codebench from Mentor Graphics.  The MIPS
toolchain archive could be downloaded
[here](sourcery.mentor.com/public/gnu_toolchain/mips-linux-gnu/mips-2015.05-18-mips-linux-gnu-i686-pc-linux-gnu.tar.bz2).

`$PATH` (and `$LD_LIBRARY_PATH` for Loongson's compiler) should be modified
accordingly to contain the binaries and libraries provided.
Read `/etc/profile` and put the paths into appropriate files and/or locations.

(TODO: add third-party ARM cross-compiler providers)

#### Build from source

(TODO: write source build instructions for ARM/IA32/MIPS)
