
/*
 * NOTE: This header only contains statements that should be moved to other
 * headers.
 *
 * You should not include this header.
 */

/* A register takes 2 ^ (WORD_SHIFT) bytes */
#if defined(_MIPS_ARCH_MIPS32) || defined(_MIPS_ARCH_MIPS32R2)
#define WORD_SHIFT	2
#elif defined(_MIPS_ARCH_MIPS64) || defined(_MIPS_ARCH_MIPS64R2)
#define WORD_SHIFT	3
#endif
