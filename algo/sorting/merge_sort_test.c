#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "merge_sort.h"
#include "array.h"

int main(int argc, char* argv[]) {

    int n = atoi(argv[1]);

    int* arr = (int*)malloc(n * sizeof(int));
    
    array_random(arr, n);

    struct timespec t_start;
    struct timespec t_finish;

    clock_gettime(CLOCK_MONOTONIC, &t_start);

    merge_sort(arr, 0, n-1);

    clock_gettime(CLOCK_MONOTONIC, &t_finish);

    int check = array_check_sorting(arr, n);

    printf("check: %d\n", check);

    int sec_elapsed = t_finish.tv_sec - t_start.tv_sec;
    long long nsec_elapsed = t_finish.tv_nsec - t_start.tv_nsec;

    double elapsed = sec_elapsed + nsec_elapsed * 0.000000001;

    printf("Elapsed time: %f\n", elapsed);
    
    return 0;
}