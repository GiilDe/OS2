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
#define SYSCALL_CHANGE 245
#define SYSCALL_GET_POLICY 246

int handle_res_code(int res) {
    if((res) < 0) {
        errno = (-res);
        return -1;
    }

    return res;
}

int is_changeable(pid_t pid) {
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_IS_CHANGEABLE), "b" (pid)
        : "memory"
        );
    return handle_res_code(res);
}

int make_changeable(pid_t pid) {
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_MAKE_CHANGEABLE), "b" (pid)
        : "memory"
        );
    return handle_res_code(res);
}

int change(int val) {
    int res;
    __asm__(
    "int $0x80;"
    : "=a" (res)
    : "0" (SYSCALL_CHANGE), "b" (val)
    : "memory"
    );
    return handle_res_code(res);
}

int get_policy(pid_t pid) {
    int res;
    __asm__(
        "int $0x80;"
        : "=a" (res)
        : "0" (SYSCALL_GET_POLICY), "b" (pid)
        : "memory"
    );
    return handle_res_code(res);
}

int get_changeables_num() {
    int res;
    __asm__(
    "int $0x80;"
    : "=a" (res)
    : "0" (247)
    : "memory"
    );
    return handle_res_code(res);
}

#endif //OS1_HW1_SYSCALLS_H
