#include <stdio.h>
#ifdef DYNAMIC
#include <dlfcn.h>
#else
#include "libcollatz.h"
#endif

int main(void) {
    int numbers[] = {20,100,123, 200,423, 562};
    int length = sizeof(numbers) / sizeof(numbers[0]);
    int max_iter = 30;
    int a[30];
    int temp;
    #ifdef DYNAMIC
    void *handle = dlopen("./libcollatz.so", RTLD_LAZY);
    if (!handle) {
        return 1;
    }
    int (*test_collatz_convergence)(int, int*, int);
    *(void **) (&test_collatz_convergence) = dlsym(handle, "test_collatz_convergence");
    if (dlerror()) {
        dlclose(handle);
        return 1;
    }
    #endif    
    for (int i = 0; i < length; i++){
        temp = test_collatz_convergence(numbers[i], a, max_iter);
        if (temp){
            printf("%4d | %5d | ", numbers[i], temp);
            for( int j = 0; j < temp; j++){
                printf("%d ", a[j]);
            }
            printf("\n");
        }
        else
            printf("%4d | error\n", temp);
    }
    #ifdef DYNAMIC
    dlclose(handle);
    #endif    
}