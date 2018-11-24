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

    return info->is_changeable;
}

/**
 * Make a given process CHANGEABLE
 * @param pid The process ID of the given process
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int make_changeable(pid_t pid){
    struct task_struct* target_p = find_task_by_pid(pid);

    if(target_p == NULL) {
        return -ESRCH;
    }

    if(current->is_changeable || target_p->is_changeable) {
        return -EINVAL;
    }

    target_p->is_changeable = 1;
    return 0;
}

int sys_change(int val){

    // TODO Implement
    return 0;
}

/**
 *
 * @param pid The process ID
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int sys_get_policy(pid_t pid){
    // TODO Implement
    return 0;
}