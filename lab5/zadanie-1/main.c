#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handler(const int signal) {
    printf("Signal %d", signal);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "none") == 0) {
    }
    if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    }
    else if (strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handler);
    }
    else if (strcmp(argv[1], "mask") == 0) {
        sigset_t sigset;
        sigemptyset(&sigset);
        sigaddset(&sigset, SIGUSR1);
        sigprocmask(SIG_SETMASK, &sigset, NULL);
    }
    else {
        return EXIT_FAILURE;
    }

    raise(SIGUSR1);
    if (strcmp(argv[1], "mask")==0) {
        sigset_t sigsetwaiting;
        sigpending(&sigsetwaiting);
        if (sigismember(&sigsetwaiting, SIGUSR1)) {
            printf("SIGUSR1 is waiting");
        } else printf("No signal is waiting");
    }
    return EXIT_SUCCESS;
}
