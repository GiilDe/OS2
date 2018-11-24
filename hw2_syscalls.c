//
// Created by Miki Mints on 11/7/18.
//
#include<linux/kernel.h>
#include<linux/sched.h>
#include<linux/errno.h>
#include<linux/slab.h>
#include<asm/uaccess.h>

//typedef struct runqueue runqueue_t;
//
//struct runqueue {
//    spinlock_t lock;
//    unsigned long nr_running, nr_switches, expired_timestamp;
//    signed long nr_uninterruptible;
//    task_t *curr, *idle;
//    prio_array_t *active, *expired, arrays[2];
//    prio_array_t changeables;
//    int prev_nr_running[NR_CPUS];
//    task_t *migration_thread;
//    list_t migration_queue;
//} ____cacheline_aligned;


int sys_is_changeable(pid_t pid){
    struct task_struct* info = find_task_by_pid(pid);

    if(info == NULL) {
        return -ESRCH;
    }

    return info->is_changeable;
}

/**
 * Make a given process CHANGEABLE
 * @param pid The process ID of the given process
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int sys_make_changeable(pid_t pid){
    struct task_struct* target_p = find_task_by_pid(pid);

    if(target_p == NULL) {
        return -ESRCH;
    }

    if(current->is_changeable || target_p->is_changeable) {
        return -EINVAL;
    }

    target_p->policy = SCHED_CHANGEABLE;
    target_p->is_changeable = 1;

//    runqueue_t *rq = this_rq();
//    spin_lock_irq(rq);
//
//    list_t current_changeable = rq->changeables.queue[0];
//    list_add_tail(&target_p->run_list, &current_changeable);
//
//    spin_unlock_irq(rq);

    return 0;
}

int sys_change(int val){
    if(val != 1 && val != 0){
        return -EINVAL;
    }
    is_changeable_enabled = val;
    return 0;
}

/**
 *
 * @param pid The process ID
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int sys_get_policy(pid_t pid){
    struct task_struct* target_p = find_task_by_pid(pid);

    if(target_p == NULL) {
        return -ESRCH;
    }

    return target_p->is_changeable && is_changeable_enabled;
}