#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void array_split(int* in, int l, int r, int* out){

    // Copia in out i valori di in dalla posizione l (compresa) alla posizione r (non compresa)
    for (int i = 0; i < r - l; i++) {
        out[i] = in[l + i];
    }

}

void array_copy(int*in, int* out, int n) {
    for(int i = 0; i < n; i++) {
        out[i] = in[i];
    }
}

void array_print(int* arr, int n) {
    printf("[");
    for (int i = 0; i < n; i++) {
        printf("%d, ", arr[i]);
    }
    printf("]\n");
}

void array_random(int* arr, int n) {

    // Inizializza il generatore di numeri casuali
    srand(time(NULL));

    // Riempie l'array con numeri casuali (0-n)
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % n;
    }
    
}

void array_reduce(int* arr, int n) {
    int j = 0; 
    for (int i = 0; i < n; i++) {
        if (i % 2 == 1) {
            arr[j] = arr[i] + arr[i - 1];
            j++;
        } else if (i % 2 == 0 && i + 1 == n) {
            arr[j] = arr[i];
        }
    }
}