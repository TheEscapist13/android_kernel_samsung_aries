/* linux/arch/arm/mach-s5pv210/include/mach/system.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com/
 *
 * S5PV210 - system support header
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H __FILE__

<<<<<<< HEAD
#include <asm/proc-fns.h>
#include <plat/watchdog-reset.h>

static void arch_idle(void)
{
	/* nothing here yet */
}
=======
#include <plat/system-reset.h>
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static void arch_idle(void)
{
	if (mode != 's')
		arch_wdt_reset();

	/* if all else fails, or mode was for soft, jump to 0 */
	cpu_reset(0);
}

#endif /* __ASM_ARCH_SYSTEM_H */
