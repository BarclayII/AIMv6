
/*
 * Note: this piece of code runs in user space, although being a portion
 * of kernel.
 *
 * Later we'll use this piece of user space code to bootstrap /sbin/init
 * program.
 */

#include <stdio.h>

int main(void)
{
	/* https://en.wikipedia.org/wiki/Poland_can_into_space */
	printf("We can into userspace!\n");
	__tokernel();		/* __tokernel() is a system call wrapper */
	/* NOTREACHED */
	return 0;
}
