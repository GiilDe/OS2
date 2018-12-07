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

    if(info == NULL || info->state == TASK_ZOMBIE) {
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

    if(current->policy == SCHED_CHANGEABLE || target_p->policy == SCHED_CHANGEABLE || target_p->policy != SCHED_OTHER || target_p->state == TASK_ZOMBIE) {
        return -EINVAL;
    }

    target_p->policy = SCHED_CHANGEABLE;
    enqueue_changeable_and_count_locked(target_p);
    if(current == target_p && is_changeable_locked(current)) {
        update_running_process();
    }
    return 0;
}

int sys_change(int val){
    if(val != 1 && val != 0){
        return -EINVAL;
    }
    if(!is_changeables_empty_locked()) {
        set_is_changeable_enabled_locked(val);
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

    if(target_p->policy != SCHED_CHANGEABLE || target_p->state == TASK_ZOMBIE) {
        return -EINVAL;
    }

    return is_changeable_locked(target_p);
}

int sys_get_changeables_num(){
    return get_changeables_num();
}