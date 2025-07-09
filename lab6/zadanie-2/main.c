#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
    double a,b;
    const char *path1 = "input_fifo";
    const char *path2 = "result_fifo";
    mkfifo(path1, 0666);
    mkfifo(path2, 0666);
    scanf("%lf %lf", &a, &b);
    int fd = open(path1, O_WRONLY);
    write(fd, &a, sizeof(double));
    write(fd, &b, sizeof(double));
    close(fd);
    fd = open(path2, O_RDONLY);
    double result;
    read(fd, &result, sizeof(double));
    printf("Result: %lf", result);
    remove(path1);
    remove(path2);
}
