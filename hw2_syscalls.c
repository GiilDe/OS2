//
// Created by Miki Mints on 11/7/18.
//
#include<linux/kernel.h>
#include<linux/sched.h>
#include<linux/errno.h>
#include<linux/slab.h>
#include<asm/uaccess.h>


int sys_is_changeable(pid_t pid){
    struct task_struct* info = find_task_by_pid(pid);

    if(info == NULL) {
        return -ESRCH;
    }

    return info->policy == SCHED_CHANGEABLE;
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

    if(current->policy == SCHED_CHANGEABLE || target_p->policy == SCHED_CHANGEABLE) {
        return -EINVAL;
    }

    target_p->policy = SCHED_CHANGEABLE;
    enqueue_changeable_locking(target_p);
    return 0;
}

int sys_change(int val){
    if(val != 1 && val != 0){
        return -EINVAL;
    }
    if(!is_changeables_empty()) {
        // Start the regime only if there are currently CHANGEABLE processes in the system
        is_changeable_enabled = val;
    }
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

    if(target_p->policy != SCHED_CHANGEABLE) {
        return -EINVAL;
    }

    return is_changeable(target_p);
}