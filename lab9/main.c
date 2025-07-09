#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
typedef struct  {
    double a,b, h;
    double *result_array_ptr;
} Integral_args;

double f(const double x) {
    return 4/(x*x+1);
}
void *midpoint_integral(void *args) {
    Integral_args* integral_args = args;
    double a = integral_args->a;
    double b = integral_args->b;
    double h = integral_args->h;
    double res = 0;
    for (double x = a +h/2; x<b; x+=h ) {
        res += f(x) *h;
    }
    *(integral_args->result_array_ptr) = res;
    free(integral_args);
    return NULL;
}

int main(int argc, char *argv[]){
    double h = atof(argv[1]);
    int k = atoi(argv[2]);

    double *result_array = malloc(sizeof(double) * k);
    pthread_t *threads = malloc(sizeof(pthread_t) * k);
    double *first = result_array;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i=0;i<k; i++)
    {
        Integral_args *tmp = malloc(sizeof(Integral_args));
        tmp->a =(double)i / k;
        tmp->b= (double)(i+1) / k;
        tmp->h = h;
        tmp->result_array_ptr = first++;
        pthread_create(&threads[i], NULL, midpoint_integral, tmp);
    }

    double sum = 0;
    for (int i = 0; i<k; i++ ) {
        pthread_join(threads[i], NULL);
        sum += result_array[i];
    }
    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1e6;
    printf("%2d|%1.4f|%2.5f\n",k, sum,(elapsed));
    free(threads);

    free(result_array);
}