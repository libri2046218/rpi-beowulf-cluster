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


