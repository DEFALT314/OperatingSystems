#include <signal.h>
#include <stdio.h>
#include <unistd.h>
int count = 0;
int mode = 1;
void siginthandler(int sig) {
    printf("Wciśnięto CTRL+C\n");
}
void handler(int sig_num, siginfo_t *siginfo, void *context)  {
    mode = siginfo->si_value.sival_int;
    pid_t sender = siginfo->si_pid;
    kill(sender, SIGUSR1);
    count++;
    switch (mode) {
        case 1:
            printf("Calls count: %d\n", count);
            break;
        case 2:
            break;
        case 3:
            signal(SIGINT, SIG_IGN);
            break;
        case 4:
            signal(SIGINT, siginthandler);
            break;
    }

}
int main() {
    printf("PID: %d\n", getpid());
    struct sigaction act;
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    while (mode != 5) {
        if (mode ==2) {
            int n = 0;
            while (mode ==2) {
                printf("%d\n", n++);
                sleep(1);
            }
        }
    }
}
