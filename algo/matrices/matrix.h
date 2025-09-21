#ifndef MATRIX_H
#define MATRIX_H

typedef struct {

	int* M;
	int n;

} Matrix;

Matrix* matrix_alloc(int n);

void matrix_free(Matrix* m);

void matrix_print(Matrix* m);

void matrix_get(Matrix* mat, int row, int col, int* out);

void matrix_set(Matrix* mat, int row, int col, int in);

void matrix_transpose(Matrix* mat);

void matrix_random(Matrix* out);

void matrix_empty(Matrix* out);

void matrix_copy(Matrix* in, Matrix* out);

#endif
