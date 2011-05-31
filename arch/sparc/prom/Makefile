# Makefile for the Sun Boot PROM interface library under
# Linux.
#
asflags := -ansi
ccflags := -Werror

lib-y                 := bootstr_$(BITS).o
<<<<<<< HEAD
lib-$(CONFIG_SPARC32) += devmap.o
=======
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
lib-y                 += init_$(BITS).o
lib-$(CONFIG_SPARC32) += memory.o
lib-y                 += misc_$(BITS).o
lib-$(CONFIG_SPARC32) += mp.o
lib-$(CONFIG_SPARC32) += ranges.o
lib-$(CONFIG_SPARC32) += segment.o
lib-y                 += console_$(BITS).o
lib-y                 += printf.o
lib-y                 += tree_$(BITS).o
lib-$(CONFIG_SPARC64) += p1275.o
lib-$(CONFIG_SPARC64) += cif.o
