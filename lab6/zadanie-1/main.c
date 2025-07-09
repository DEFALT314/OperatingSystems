#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

double f(const double x) {
    return 4/(x*x+1);
}
double midpoint_integral(const double a, const double b, const double h ) {
    double res = 0;
    for (double x = a +h/2; x<b; x+=h ) {
        res += f(x) *h;
    }
    return res;
}
int main(int argc, char *argv[]) {
    double h = atof(argv[1]);
    int n = atoi(argv[2]);
    double a = 0.0;
    double b= 1.0;
    for (int k = 1; k<=n; k++) {
        int fd[2];
        struct timeval start, end;
        gettimeofday(&start, NULL);
        if (pipe(fd)==-1) {
            return EXIT_FAILURE;
        }
        for (int i=0; i<k; i++) {
            pid_t pid;
            if ((pid = fork()) == -1) {
                return EXIT_FAILURE;
            }
            if (pid==0) {
                close(fd[0]);
                a = (double)i/k;
                b= (double)(i+1)/k;
                double result = midpoint_integral(a,b,h);
                write(fd[1], &result, sizeof(double));
                close(fd[1]);
                return EXIT_SUCCESS;
            }
        }
        close(fd[1]);
        double result = 0.0;
        double temp;
        for (int i =0; i<k; i++) {
            read(fd[0], &temp, sizeof(double));
            result += temp;
        }
        close(fd[0]);
        for (int i=0; i<k; i++) {
            wait(NULL);
        }
        gettimeofday(&end, NULL);
        double elapsed = (end.tv_sec - start.tv_sec) +
                        (end.tv_usec - start.tv_usec) / 1e6;
        printf("%2d|%1.4f|%2.5f\n",k, result,(elapsed));

    }

}
