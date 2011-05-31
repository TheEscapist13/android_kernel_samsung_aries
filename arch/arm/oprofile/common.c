/**
 * @file common.c
 *
 * @remark Copyright 2004 Oprofile Authors
 * @remark Copyright 2010 ARM Ltd.
 * @remark Read the file COPYING
 *
 * @author Zwane Mwaikambo
 * @author Will Deacon [move to perf]
 */

#include <linux/cpumask.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <linux/oprofile.h>
#include <linux/perf_event.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <asm/stacktrace.h>
#include <linux/uaccess.h>

#include <asm/perf_event.h>
#include <asm/ptrace.h>

#ifdef CONFIG_HW_PERF_EVENTS
char *op_name_from_perf_id(void)
{
<<<<<<< HEAD
	int id;
	u32 cpu = smp_processor_id();

	for (id = 0; id < perf_num_counters; ++id)
		if (perf_events[cpu][id] == event)
			break;

	if (id != perf_num_counters)
		oprofile_add_sample(regs, id);
	else
		pr_warning("oprofile: ignoring spurious overflow "
				"on cpu %u\n", cpu);
}

/*
 * Called by op_arm_setup to create perf attributes to mirror the oprofile
 * settings in counter_config. Attributes are created as `pinned' events and
 * so are permanently scheduled on the PMU.
 */
static void op_perf_setup(void)
{
	int i;
	u32 size = sizeof(struct perf_event_attr);
	struct perf_event_attr *attr;

	for (i = 0; i < perf_num_counters; ++i) {
		attr = &counter_config[i].attr;
		memset(attr, 0, size);
		attr->type		= PERF_TYPE_RAW;
		attr->size		= size;
		attr->config		= counter_config[i].event;
		attr->sample_period	= counter_config[i].count;
		attr->pinned		= 1;
	}
}

static int op_create_counter(int cpu, int event)
{
	int ret = 0;
	struct perf_event *pevent;

	if (!counter_config[event].enabled || (perf_events[cpu][event] != NULL))
		return ret;

	pevent = perf_event_create_kernel_counter(&counter_config[event].attr,
						  cpu, -1,
						  op_overflow_handler);

	if (IS_ERR(pevent)) {
		ret = PTR_ERR(pevent);
	} else if (pevent->state != PERF_EVENT_STATE_ACTIVE) {
		perf_event_release_kernel(pevent);
		pr_warning("oprofile: failed to enable event %d "
				"on CPU %d\n", event, cpu);
		ret = -EBUSY;
	} else {
		perf_events[cpu][event] = pevent;
	}

	return ret;
}
=======
	enum arm_perf_pmu_ids id = armpmu_get_pmu_id();
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

	switch (id) {
	case ARM_PERF_PMU_ID_XSCALE1:
		return "arm/xscale1";
	case ARM_PERF_PMU_ID_XSCALE2:
		return "arm/xscale2";
	case ARM_PERF_PMU_ID_V6:
		return "arm/armv6";
	case ARM_PERF_PMU_ID_V6MP:
		return "arm/mpcore";
	case ARM_PERF_PMU_ID_CA8:
		return "arm/armv7";
	case ARM_PERF_PMU_ID_CA9:
		return "arm/armv7-ca9";
	default:
		return NULL;
	}
}
#endif

static int report_trace(struct stackframe *frame, void *d)
{
	unsigned int *depth = d;

	if (*depth) {
		oprofile_add_trace(frame->pc);
		(*depth)--;
	}

	return *depth == 0;
}

/*
 * The registers we're interested in are at the end of the variable
 * length saved register structure. The fp points at the end of this
 * structure so the address of this struct is:
 * (struct frame_tail *)(xxx->fp)-1
 */
struct frame_tail {
	struct frame_tail *fp;
	unsigned long sp;
	unsigned long lr;
} __attribute__((packed));

static struct frame_tail* user_backtrace(struct frame_tail *tail)
{
	struct frame_tail buftail[2];

	/* Also check accessibility of one struct frame_tail beyond */
	if (!access_ok(VERIFY_READ, tail, sizeof(buftail)))
		return NULL;
	if (__copy_from_user_inatomic(buftail, tail, sizeof(buftail)))
		return NULL;

	oprofile_add_trace(buftail[0].lr);

	/* frame pointers should strictly progress back up the stack
	 * (towards higher addresses) */
	if (tail + 1 >= buftail[0].fp)
		return NULL;

	return buftail[0].fp-1;
}

static void arm_backtrace(struct pt_regs * const regs, unsigned int depth)
{
	struct frame_tail *tail = ((struct frame_tail *) regs->ARM_fp) - 1;

	if (!user_mode(regs)) {
		struct stackframe frame;
		frame.fp = regs->ARM_fp;
		frame.sp = regs->ARM_sp;
		frame.lr = regs->ARM_lr;
		frame.pc = regs->ARM_pc;
		walk_stackframe(&frame, report_trace, &depth);
		return;
	}

	while (depth-- && tail && !((unsigned long) tail & 3))
		tail = user_backtrace(tail);
}

int __init oprofile_arch_init(struct oprofile_operations *ops)
{
<<<<<<< HEAD
	int cpu, ret = 0;

	perf_num_counters = armpmu_get_max_events();

	counter_config = kcalloc(perf_num_counters,
			sizeof(struct op_counter_config), GFP_KERNEL);

	if (!counter_config) {
		pr_info("oprofile: failed to allocate %d "
				"counters\n", perf_num_counters);
		return -ENOMEM;
	}

	ret = init_driverfs();
	if (ret) {
		kfree(counter_config);
		counter_config = NULL;
		return ret;
	}

	for_each_possible_cpu(cpu) {
		perf_events[cpu] = kcalloc(perf_num_counters,
				sizeof(struct perf_event *), GFP_KERNEL);
		if (!perf_events[cpu]) {
			pr_info("oprofile: failed to allocate %d perf events "
					"for cpu %d\n", perf_num_counters, cpu);
			while (--cpu >= 0)
				kfree(perf_events[cpu]);
			return -ENOMEM;
		}
	}

=======
	/* provide backtrace support also in timer mode: */
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
	ops->backtrace		= arm_backtrace;

	return oprofile_perf_init(ops);
}

<<<<<<< HEAD
void oprofile_arch_exit(void)
{
	int cpu, id;
	struct perf_event *event;

	if (*perf_events) {
		for_each_possible_cpu(cpu) {
			for (id = 0; id < perf_num_counters; ++id) {
				event = perf_events[cpu][id];
				if (event != NULL)
					perf_event_release_kernel(event);
			}
			kfree(perf_events[cpu]);
		}
	}

	if (counter_config) {
		kfree(counter_config);
		exit_driverfs();
	}
}
#else
int __init oprofile_arch_init(struct oprofile_operations *ops)
=======
void __exit oprofile_arch_exit(void)
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
{
	oprofile_perf_exit();
}
