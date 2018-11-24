//
// Created by Gilad on 07-Nov-18.
//

#ifndef OS1_HW1_SYSCALLS_H
#define OS1_HW1_SYSCALLS_H

#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>

#define SYSCALL_IS_CHANGEABLE 243
#define SYSCALL_MAKE_CHANGEABLE 244
#define SYSCALL_SET_PROCESS_CAPABILITIES 245
#define SYSCALL_GET_PROCESS_LOG 246


typedef struct forbidden_activity_info{
    int syscall_req_level;
    int proc_level;
    int time;
} log_record;

int handle_res_code(int res) {
    if((res) < 0) {
        errno = (-res);
        return -1;
    }

    return res;
}

int is_changeable(pid_t pid){
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_IS_CHANGEABLE), "b" (pid)
        : "memory"
        );
    return handle_res_code(res);
}

int make_changeable(pid_t pid){
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_DISABLE_POLICY), "b" (pid)
        : "memory"
        );
    return handle_res_code(res);
}

int set_process_capabilities(pid_t pid, int new_level, int password){
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_SET_PROCESS_CAPABILITIES), "b" (pid), "c" (new_level), "d" (password)
        : "memory"
        );
    return handle_res_code(res);
}

int get_process_log(pid_t pid, int size, struct forbidden_activity_info* user_mem) {
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_GET_PROCESS_LOG), "b" (pid), "c" (size), "d" (user_mem)
        : "memory"
    );
    return handle_res_code(res);
}

#endif //OS1_HW1_SYSCALLS_H
