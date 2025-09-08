#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

void matrix_multiplication(Matrix* A, Matrix* B, Matrix* C, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			int val;
			matrix_get(C, i, j, &val);

			for (int x = 0; x < n; x++) {
				int a_aux;
				int b_aux;

				matrix_get(A, i, x, &a_aux);
				matrix_get(B, x, j, &b_aux);

				val += a_aux * b_aux;
			}

			matrix_set(C, i, j, val);
		}
	}
}

int main(int argc, char* argv[]) {

	srand(time(NULL));

	int n = atoi(argv[1]);

	Matrix* A = matrix_alloc(n);
	matrix_random(A);
	Matrix* B = matrix_alloc(n);
	matrix_random(B);
	Matrix* C = matrix_alloc(n);
	matrix_empty(C);
	
	matrix_print(A);
	matrix_print(B);
	matrix_print(C);

	matrix_multiplication(A,B,C,n);

	matrix_print(C);

	matrix_transpose(C);

	matrix_print(C);

	matrix_free(A);
	matrix_free(B);
	matrix_free(C);
	
	return 0;

}
