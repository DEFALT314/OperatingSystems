#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    int childNumber = atoi(argv[1]);
    for (int i=0; i< childNumber; i++){
        pid_t pid = fork();
        if(pid ==0){
            printf("child pid = %d, parent pid = %d\n", (int)getpid(), (int)getppid());
            exit(0);
        }
    }
    for (int i=0; i< childNumber; i++){
        wait(NULL);
    }
    printf("%d", childNumber);
}