//
// Created by Miki Mints on 12/5/18.
//
#include "macros.h"
#include "hw2_syscalls.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sched.h>
#include <list>
#include <algorithm>

bool testChange() {
    // Process is not SC
    ASSERT_FALSE(is_changeable(getpid()));
    ASSERT_EQUALS(get_policy(getpid()), -1);
    ASSERT_EQUALS(errno, EINVAL);

    for (int i = 0; i < 100; ++i) {
        if (CHILD_PROCESS(fork())) {
            CHILD_ASSERT_EQUALS(get_policy(getpid()), -1);
            CHILD_EXIT();
        }
    }
    change(1);
    for (int i = 0; i < 100; ++i) {
        if (CHILD_PROCESS(fork())) {
            CHILD_ASSERT_TRUE(get_policy(getpid()));
            CHILD_EXIT();
        }
    }

    while (wait(NULL) > 0); // Wait for all children to finish
    // Policy should be turned off
    // TODO Fix
    ASSERT_EQUALS(make_changeable(getpid()), 0);
    ASSERT_FALSE(get_policy(getpid()));
    return true;
}

bool testIsChangeable() {
    ASSERT_FALSE(is_changeable(getpid()));

    int pid = fork();
    if(CHILD_PROCESS(pid)) {
        int status = is_changeable(getpid());
        CHILD_ASSERT_FALSE(status);
        change(1);
        // is_changeable should only test for process' policy
        CHILD_ASSERT_FALSE(status);
        CHILD_EXIT();
    } else {
        int code = 0;
        WAIT_CHILD(code);
    }

    ASSERT_EQUALS(make_changeable(getpid()), 0);
    ASSERT_TRUE(is_changeable(getpid()));

    pid = fork();
    if(CHILD_PROCESS(pid)) {
        CHILD_ASSERT_TRUE(is_changeable(getpid()));
        CHILD_EXIT();
    } else {
        int code = 0;
        WAIT_CHILD(code);
        // Child process does not exist anymore
        ASSERT_EQUALS(is_changeable(pid), -1);
        ASSERT_EQUALS(errno, ESRCH);
    }

    ASSERT_EQUALS(is_changeable(-1), -1);
    ASSERT_EQUALS(errno, ESRCH);

    return true;
}

bool testMakeChangeable() {
    int ppid = getpid();
    int pid = fork();
    int pid2 = fork();
    if(CHILD_PROCESS(pid2)) {
        // Second child
        // Keep this child alive until both Father and Second Child finish
        int code = 0;
        waitpid(ppid, NULL, 0);
        CHILD_EXIT();
    } else {
        if(CHILD_PROCESS(pid)) {
            // First child
            CHILD_ASSERT_EQUALS(make_changeable(pid2), -1);
            CHILD_ASSERT_EQUALS(errno, EINVAL); // First + Second child are CHANGEABLE

            // Before First Child finishes, change the Second Child to RR (Real time)
            // Also, change the father back to FIFO
            struct sched_param param;
            param.sched_priority = 99;
            CHILD_ASSERT_EQUALS(sched_setscheduler(pid2, SCHED_RR, &param), 0);

            struct sched_param param2;
            param2.sched_priority = 0;
            CHILD_ASSERT_EQUALS(sched_setscheduler(ppid, SCHED_OTHER, &param2), 0);

            CHILD_EXIT();
        } else {
            // Father process
            int code = 0;
            WAIT_CHILD_PID(code, pid);
            // First Child finished
            // Father == SC, Second Child == RR
            ASSERT_EQUALS(make_changeable(pid2), -1);
            ASSERT_EQUALS(errno, EINVAL); // Can't change Real Time processes

            ASSERT_FALSE(is_changeable(getpid()));
            ASSERT_EQUALS(make_changeable(getpid()), 0); // Father should be OTHER now
            ASSERT_TRUE(is_changeable(getpid()));

            ASSERT_EQUALS(make_changeable(-1), -1);
            ASSERT_EQUALS(errno, ESRCH);
            ASSERT_EQUALS(make_changeable(65000), -1);
            ASSERT_EQUALS(errno, ESRCH);
        }
    }
    return true;
}

bool testChangeAndGetPolicy() {
    // Father process is still CHANGEABLE
    ASSERT_TRUE(is_changeable(getpid()));

    ASSERT_EQUALS(change(-1), -1);
    ASSERT_EQUALS(errno, EINVAL);

    ASSERT_EQUALS(change(65000), -1);
    ASSERT_EQUALS(errno, EINVAL);

    // No regime change here
    ASSERT_EQUALS(change(0), 0);
    ASSERT_EQUALS(change(1), 0);

    std::list<int> pids;
    int ppid = getpid();
    ASSERT_TRUE(get_policy(getpid()));
    ASSERT_EQUALS(get_policy(getppid()), -1); // Parent is not SC
    ASSERT_EQUALS(errno, EINVAL);
    for (int i = 0; i < 50; ++i) {
        int p = fork();
        pids.push_front();
        if (CHILD_PROCESS(p)) {
            // All child processes busy-wait for father to finish
            CHILD_ASSERT_TRUE(get_policy(getpid()));
            waitpid(ppid, NULL, 0);
            CHILD_EXIT();
        }
    }

    ASSERT_EQUALS(get_policy(-1), -1);
    ASSERT_EQUALS(errno, ESRCH);

    return true;
}

int main() {
    int p = fork();
    int status = 0;
    if(CHILD_PROCESS(p)) {
        // Test change semantics separately
        RUN_TEST(testChange);
        CHILD_EXIT();
    } else {
        waitpid(p, &status, 0);
        RUN_TEST(testIsChangeable);
        RUN_TEST(testMakeChangeable);
        RUN_TEST(testChangeAndGetPolicy);
    }

    if(status == 0) {
        cout << GREEN << "[SUCCESS] Functional tests passed!" << NORMAL_TEXT << endl;
    }
    return 0;

}
