#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    if (argc != 3) {
        exit(EXIT_FAILURE);
    }
    pid_t pid = atoi(argv[1]);
    int mode = atoi(argv[2]);
    sigset_t new_mask, old_mask;
    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);
    union sigval sigval;
    sigval.sival_int = mode;
    if (sigqueue(pid,SIGUSR1,sigval) ==-1) {
        exit(EXIT_FAILURE);
    }
    sigsuspend(&old_mask);
}
