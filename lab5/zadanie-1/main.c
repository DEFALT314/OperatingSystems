

#include <stdio.h>

void handler(int signal) {
    printf("Signal %d", signal);
}
int main(int argc, char *argv[]) {
    printf()
    switch (argv[0]) {
        case "none":
            break;
        case "ignore":
            signal(SIGUSR1, SIG_IGN);
            break;
        case "handler":
            signal(SIGUSR1, handler);
            break;
        case "ign":
            sigset_t sigset;
            sigemptyset(&sigset);
            sigprocmask(SIG_SETMASK, );
    }
}