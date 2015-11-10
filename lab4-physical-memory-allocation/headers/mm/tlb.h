
#ifndef _ASM_MM_TLB_H
#define _ASM_MM_TLB_H

/* C functions corresponding to TLB instructions */

#define tlbp()		asm volatile ("tlbp")
#define tlbr()		asm volatile ("tlbr")
#define tlbwr()		asm volatile ("tlbwr")
#define tlbwi()		asm volatile ("tlbwi")

/* TLB "physical" entries (ENTRYLO0 and ENTRYLO1) */
#define ENTRYLO_PFN(entrylo)	((entrylo) >> 6)	/* PFN */
#define ENTRYLO_GLOBAL		0x1		/* ignore ASID */
#define ENTRYLO_VALID		0x2		/* valid */
#define ENTRYLO_DIRTY		0x3		/* writable */
#define ENTRYLO_CACHEMASK	0x38
# define ENTRYLO_UNCACHED	0x10		/* uncached access */
# define ENTRYLO_CACHE_COHERENT	0x18		/* cache coherent */

/* TLB "virtual" entries (ENTRYHI) */
#define ENTRYHI_ASID_MASK	0xff
#define ENTRYHI_KSU_MASK	0xc0000000
#define ENTRYHI_ASID(entryhi)	((entryhi) & ENTRYHI_ASID_MASK)
#define ENTRYHI_KSU(entryhi)	((entryhi) & ENTRYHI_KSU_MASK)
/*
 * You should probably write a function/macro for obtaining VPN
 * by your own, according to your design.
 */

#endif
