#include <errno.h>
#include <sched.h>
#include "hw2_syscalls.h"
#include <sys/types.h>
#include <unistd.h>
#include "stdio.h"
#include "assert.h"
#include "stdio.h"
int main(){
    assert(is_changeable(-1)==-1);
    assert(errno==ESRCH);
    assert(is_changeable(getpid()==0));
    assert(make_changeable(-1)==-1);
    assert(errno==ESRCH);
    assert(change(-1)==-1);
    assert(errno==EINVAL);
    assert(change(2)==-1);
    assert(errno==EINVAL);
    assert(change(0)==0);
    assert(change(1)==0);
    assert(get_policy(-1)==-1);
    assert(errno==ESRCH);
    assert(get_policy(getpid())==-1);
    assert(errno==EINVAL);
    pid_t pid = fork();
    if(pid==0){//son
        assert(make_changeable(getpid())==0);//son process is now changeable
        assert(make_changeable(getppid())==-1);//caller is changeable
        assert(errno==EINVAL);
        assert(get_policy(getpid())==0);//policy is off since no sc process were available when change(1) called
        assert(is_changeable(getpid())==1);
        assert(is_changeable(getppid())==0);
         assert(change(1)==0);
        assert(get_policy(getpid())==1);
        return 0;
    }
    else{//father
         wait(NULL);//no son anymore
        assert(make_changeable(pid)==-1);//pid not excist
        assert(errno==ESRCH);//son process is dead
        assert(is_changeable(getpid())==0);//father is not changeable
        assert(make_changeable(getpid())==0);//father is changeable,policy off
        assert(get_policy(getpid())==0);
        assert(change(1)==0);
        assert(get_policy(getpid())==1);
    }
    FILE* file = fopen("check.txt","w");
    if(file==NULL){
        printf("failed to lunch file");
        return 0;
    }
    int i;
    for(i=0;i<6;i++){
        fork();
    }
    fprintf(file,"%d\n",getpid());
    return 0;
}
