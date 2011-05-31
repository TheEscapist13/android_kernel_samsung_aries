#ifdef CONFIG_SCHED_AUTOGROUP

<<<<<<< HEAD
static void __sched_move_task(struct task_struct *tsk, struct rq *rq);
=======
struct autogroup {
	/*
	 * reference doesn't mean how many thread attach to this
	 * autogroup now. It just stands for the number of task
	 * could use this autogroup.
	 */
	struct kref		kref;
	struct task_group	*tg;
	struct rw_semaphore	lock;
	unsigned long		id;
	int			nice;
};
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static inline struct task_group *
autogroup_task_group(struct task_struct *p, struct task_group *tg);

#else /* !CONFIG_SCHED_AUTOGROUP */

static inline void autogroup_init(struct task_struct *init_task) {  }
<<<<<<< HEAD
=======
static inline void autogroup_free(struct task_group *tg) { }
static inline bool task_group_is_autogroup(struct task_group *tg)
{
	return 0;
}
>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6

static inline struct task_group *
autogroup_task_group(struct task_struct *p, struct task_group *tg)
{
	return tg;
}

<<<<<<< HEAD
=======
#ifdef CONFIG_SCHED_DEBUG
static inline int autogroup_path(struct task_group *tg, char *buf, int buflen)
{
	return 0;
}
#endif

>>>>>>> af0d6a0a3a30946f7df69c764791f1b0643f7cd6
#endif /* CONFIG_SCHED_AUTOGROUP */
