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
    struct task_struct* info = find_task_by_pid(pid);

    if(info == NULL) {
        return -ESRCH;
    }

    if(info->is_changeable) {
        return -EINVAL;
    }

    info->is_changeable = 1;
    return 0;
}

int sys_change(int val){
    return 0;
}

/**
 * Set a new privilege level for a process
 * @param pid The process ID
 * @param new_level The new privilege level (between 0 and 2)
 * @param password Administrator password
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int sys_set_process_capabilities(pid_t pid, int new_level, int password){
    if(pid < 0)
        return -ESRCH;

    struct task_struct* info = find_task_by_pid(pid);

    if(info == NULL) {
        return -ESRCH;
    }

    if(new_level < 0 || new_level > 2) {
        return -EINVAL;
    }

    if(password != PASSWORD) {
        return -EINVAL;
    }

    if(!info->policy_enabled){
        return -EINVAL;
    }

    printk("Setting policy privilege %d for process %d\n", new_level, pid);
    info->privilege = new_level;
    return 0;
}

/**
 * Removes the first {@param size} activity logs from a given process, and returns them
 * @param pid The process ID
 * @param size The number of activity logs to return
 * @param user_mem An array of forbidden activity logs where the logs will be returned
 * @return 0 for success, otherwise returns -errno with a given error code
 */
int sys_get_process_log(pid_t pid, int size, struct forbidden_activity_info*
                        user_mem) {
    if (pid < 0) {
        return -ESRCH;
    }

    struct task_struct *info = find_task_by_pid(pid);

    if (info == NULL) {
        return -ESRCH;
    }

    if (!info->policy_enabled || size < 0 || size > info->log_array_size || size > info->num_logs) {
        return -EINVAL;
    }

    copy_to_user(user_mem, info->log_array,
                 sizeof(struct forbidden_activity_info)*size);

    int new_size = info->log_array_size - size;
    log_record *temp = kmalloc(sizeof(struct forbidden_activity_info)*new_size, GFP_KERNEL);

    if(temp == NULL) {
        return -ENOMEM;
    }

    printk("Removing %d activity logs from process %d\n", size, pid);
    int i;
    for (i = size; i < info->num_logs; ++i) {
        temp[i] = info->log_array[i];
    }

    kfree(info->log_array);
    info->log_array = temp;
    info->num_logs -= size;
    info->log_array_size = new_size;
    return 0;
}