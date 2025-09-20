#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix.h"

Matrix* matrix_alloc(int n) {

	Matrix* ret = (Matrix*)malloc(sizeof(Matrix));
	
	int* M = (int*)malloc(n * n * sizeof(int));
	
	ret->M = M;
	ret->n = n;
	
	return ret;
}

void matrix_free(Matrix* m) {
	int n = m->n;

	free(m->M);
	
	free(m);
}

void matrix_get(Matrix* mat, int row, int col, int* out){

	int offset = row * mat->n + col;

	*out = (mat->M)[offset];

}

void matrix_set(Matrix* mat, int row, int  col, int in) {
	int offset = row * mat->n + col;
	(mat->M)[offset] = in;
}

void matrix_transpose(Matrix* mat) {
	int n = mat->n;
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < i; j++) {
			int aux_1;
			int aux_2;
			matrix_get(mat, i, j, &aux_1);
			matrix_get(mat, j, i, &aux_2);
			matrix_set(mat, i, j, aux_2);
			matrix_set(mat, j, i, aux_1);
		}
	}
}

void matrix_print(Matrix* m) {
	int n = m->n;
	
	printf("(\n");
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			int val;
			matrix_get(m, i, j, &val);
			printf("%d, ", val);
		}
		printf("\n");
	}
	printf(")\n");
}

void matrix_random(Matrix* out) {
	
	int n = out->n;

	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			matrix_set(out, i, j, rand() % n);
		}
	}
}

void matrix_empty(Matrix* out) {

	int n = out->n;
	
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			matrix_set(out, i, j, 0);
		}
	}
}

void matrix_copy(Matrix* in, Matrix* out) {
	int n = out->n;
	int aux;
	
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			matrix_get(in, i, j, &aux);
			matrix_set(out, i, j, aux);
		}
	}
	
}

