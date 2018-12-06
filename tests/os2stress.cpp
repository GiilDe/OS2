//
// Created by Miki Mints on 12/6/18.
//

#include "macros.h"
#include "hw2_syscalls.h"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <sched.h>
#include <string>
#include <cstdio>
#include <fstream>

using namespace std;

#define STRESS_CAP_1 100
#define STRESS_CAP_2 1000
#define STRESS_CAP_3 10000000
#define STRESS_CAP_BUSYWAIT 100000
#define TEMP_FILE "stress_tmp"

bool genericStressTest(int cap, int cap_busy_wait) {
    make_changeable(getpid());
    change(1);
    std::ofstream out(TEMP_FILE);
    int ppid = getpid();
    for (int i = 0; i < cap; ++i) {
        int pid = fork();
        if(CHILD_PROCESS(pid)) {
            for(int j = 0; j < cap_busy_wait; j++); // Perform arithmetic busy-waiting
            out << getpid() << endl;
            CHILD_EXIT();
        } else {
            // Do nothing
        }
    }
    if(getpid() != ppid) {
        // This is here for safety
        // (no process should reach here since they are exited after outputting their pid)
        CHILD_EXIT();
    } else {
        while (wait(NULL) > 0);
        // Close the file buffer after all processes finished using it
        out.close();

        ifstream inf (TEMP_FILE);

        if (!inf) {
            cout << RED "[ERROR] " NORMAL_TEXT << "Unable to open file " << TEMP_FILE << endl;
            return false; // terminate with error
        }

        int p_prev = -1;
        int p;
        while (inf >> p) {
            // Read PIDs in ascending order
            ASSERT_TRUE(p > p_prev);
            p_prev = p;
        }
        // Cleanup temp file
        remove(TEMP_FILE);
    }
    return true;
}

bool stressTest1() {
    return genericStressTest(STRESS_CAP_1, STRESS_CAP_BUSYWAIT)
            && genericStressTest(STRESS_CAP_2, STRESS_CAP_BUSYWAIT)
            && genericStressTest(STRESS_CAP_2, 10000000);
}

bool stressTest2() {
    make_changeable(getpid());
    change(1);
    for (int i = 0; i < STRESS_CAP_3; ++i) {
        ASSERT_TRUE(get_policy(getpid()));
        ASSERT_EQUALS(make_changeable(getpid()), -1);
        ASSERT_TRUE(is_changeable(getpid()));
        ASSERT_EQUALS(change(0), 0);
        ASSERT_EQUALS(change(1), 0);
    }
    return true;
}

int main() {
    RUN_TEST(stressTest1);
    RUN_TEST(stressTest2);

    cout << GREEN << "[SUCCESS] Stress tests passed!" << NORMAL_TEXT << endl;
    return 0;
}
