#include <fcntl.h>
#include <unistd.h>
const double h = 1e-6;
const char *path1 = "input_fifo";
const char *path2 = "result_fifo";
double f(const double x) {
    return 4/(x*x+1);
}
double midpoint_integral(const double a, const double b) {
    double res = 0;
    for (double x = a +h/2; x<b; x+=h ) {
        res += f(x) *h;
    }
    return res;
}
int main() {
    double a,b;
    int fd = open(path1, O_RDONLY);
    read(fd, &a, sizeof(double));
    read(fd, &b, sizeof(double));
    close(fd);
    double result = midpoint_integral(a,b);
    fd = open(path2, O_WRONLY);
    write(fd, &result, sizeof(double));
    close(fd);
}