/*
 * Debugging macro include header
 *
 * Copyright 1994-1999 Russell King
 * Copyright 2008 Cavium Networks
 * Moved from linux/arch/arm/kernel/debug.S by Ben Dooks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 */

<<<<<<< HEAD
		.macro	addruart,rx,rtmp
		mrc	p15, 0, \rx, c1, c0
		tst	\rx, #1			@ MMU enabled?
		moveq	\rx,      #0x10000000
		movne	\rx,      #0xf0000000	@ virtual base
		orr	\rx, \rx, #0x00009000
=======
		.macro	addruart,rp,rv
		mov	\rp, #0x00009000
		orr	\rv, \rp, #0xf0000000	@ virtual base
		orr	\rp, \rp, #0x10000000
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
		.endm

#include <asm/hardware/debug-pl01x.S>
