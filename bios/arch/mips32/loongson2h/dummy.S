
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cp0regdef.h>

LEAF(initddr)
	jr	ra
END(initddr)

LEAF(initcache)
	jr	ra
END(initcache)

LEAF(inittlb)
	jr	ra
END(inittlb)

