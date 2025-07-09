//
// Created by defalt on 19.05.25.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* thread_func(void* arg) {
    double* result = malloc(sizeof(double));
    *result = 3.14159;
    pthread_exit((void*) result);
}

int main() {
    pthread_t thread;
    double* thread_result;
    pthread_create(&thread, NULL, thread_func, NULL);

    void* retval;
    pthread_join(thread, &retval);

    thread_result = (double*) retval;
    printf("Wartość zwrócona przez wątek: %f\n", *thread_result);

    free(thread_result);
    return 0;
}
