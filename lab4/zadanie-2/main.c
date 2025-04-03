#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
int globalV = 0;
int main(int argc, char *argv[]){
    pid_t PID = fork();
    int stat;
    int localV = 0;
    if (PID == 0){
        printf("child process\n");
        globalV++, localV++;
        printf("child pid = %d, parent pid = %d\n", (int)getpid(), (int)getppid());
        printf("child's local = %d, child's global = %d\n", localV, globalV);
        execl("/bin/ls", "ls", argv[1], NULL);
    }
    else{
        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", (int)getpid(), (int)PID);
        wait(&stat);
        printf("Child exit code: %d\n", WEXITSTATUS(stat));
        printf("Parent's local = %d, parent's global = %d\n", localV, globalV );
    }
}