#include <stdlib.h>
#include <stdio.h>

/*
p   posizione del primo elemento
q   posizione dell'elemento intermedio
r   posizione dell'ultimo elemento
*/
void merge(int* A, int p, int q, int r){

    int n_l = q - p + 1; //length of a[p:q]
    int n_r = r - q; //length of a[q+1:r]

    int L[n_l];
    int R[n_r];

    int i = 0, j = 0;

    for (i = 0; i < n_l; i++) {
        L[i] = A[p + i];
    }

    for (j = 0; j < n_r; j++) {
        R[j] = A[q + j + 1];
    }

    i = 0; 
    j = 0;
    int k = p;

    while (i < n_l && j < n_r) {
        if (L[i] <= R[j]) {
            A[k] = L[i];
            i++;
        } else {
            A[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n_l) {
        A[k] = L[i];
        i++;
        k++;
    }

    while (j < n_r) {
        A[k] = R[j];
        j++;
        k++;
    }
}

/*
p   posizione del primo elemento
r   posizione dell'ultimo elemento
*/

int merge_sort(int* A, int p, int r) {

    if(p >= r) return -1;

    int q = (p + r) / 2;

    merge_sort(A, p, q);
    merge_sort(A, q+1, r);
    merge(A, p, q, r);

}

