
#ifndef _ASM_ATOMIC_H
#define _ASM_ATOMIC_H

#include <sys/types.h>

static inline bool atomic32_get_bit(volatile uint32_t *addr, int pos)
{
	return !!(*(volatile uint32_t *)addr & (1 << pos));
}

static inline void atomic32_set_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"1:	ll	%[reg], %[mem];"
		"	or	%[reg], %[val];"
		"	sc	%[reg], %[mem];"
		"	beqz	%[reg], 1b;"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(1 << pos)
	);
}

static inline void atomic32_change_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"1:	ll	%[reg], %[mem];"
		"	xor	%[reg], %[val];"
		"	sc	%[reg], %[mem];"
		"	beqz	%[reg], 1b;"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(1 << pos)
	);
}

static inline void atomic32_clear_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"1:	ll	%[reg], %[mem];"
		"	and	%[reg], %[val];"
		"	sc	%[reg], %[mem];"
		"	beqz	%[reg], 1b;"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(~(1 << pos))
	);
}

static inline bool atomic32_try_set_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"ll	%[reg], %[mem];"
		"or	%[reg], %[val];"
		"sc	%[reg], %[mem];"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(1 << pos)
	);
	return result;
}

static inline bool atomic32_try_change_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"ll	%[reg], %[mem];"
		"xor	%[reg], %[val];"
		"sc	%[reg], %[mem];"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(1 << pos)
	);
	return result;
}

static inline bool atomic32_try_clear_bit(volatile uint32_t *addr, int pos)
{
	bool result;
	asm volatile (
		"ll	%[reg], %[mem];"
		"and	%[reg], %[val];"
		"sc	%[reg], %[mem];"
		: [reg]"=&r"(result), [mem]"+m"(*addr)
		: [val]"ir"(~(1 << pos))
	);
	return result;
}

static inline bool atomic_get_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_get_bit(iaddr, ipos % 32);
}

static inline void atomic_set_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_set_bit(iaddr, ipos % 32);
}

static inline void atomic_change_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_change_bit(iaddr, ipos % 32);
}

static inline void atomic_clear_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_clear_bit(iaddr, ipos % 32);
}

static inline bool atomic_try_set_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_try_set_bit(iaddr, ipos % 32);
}

static inline bool atomic_try_change_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_try_change_bit(iaddr, ipos % 32);
}

static inline bool atomic_try_clear_bit(volatile void *addr, int pos)
{
	volatile uint32_t *iaddr;
	int ipos = pos + (int)((unsigned long)addr & 3) * 8;
	iaddr = (volatile uint32_t *)(((unsigned long)addr & ~3) + ipos / 32);
	return atomic32_try_clear_bit(iaddr, ipos % 32);
}

#endif
