#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_multiplication.h"

int main(int argc, char* argv[]) {

	srand(time(NULL));

	int n = atoi(argv[1]);

	Matrix* A = matrix_alloc(n);
	matrix_random(A);
	Matrix* B = matrix_alloc(n);
	matrix_random(B);
	Matrix* C = matrix_alloc(n);
	matrix_empty(C);
	
	//matrix_print(A);
	//matrix_print(B);
	//matrix_print(C);

    struct timespec t_start;
    struct timespec t_finish;

    clock_gettime(CLOCK_MONOTONIC, &t_start);

	matrix_multiplication(A,B,C,n);

    clock_gettime(CLOCK_MONOTONIC, &t_finish);

	//matrix_print(C);

    int sec_elapsed = t_finish.tv_sec - t_start.tv_sec;
    long long nsec_elapsed = t_finish.tv_nsec - t_start.tv_nsec;

    double elapsed = sec_elapsed + nsec_elapsed * 0.000000001;

    printf("Elapsed %f seconds\n", elapsed);

	matrix_free(A);
	matrix_free(B);
	matrix_free(C);
	
	return 0;

}