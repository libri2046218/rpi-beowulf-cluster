#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "array.h"
#include "merge_sort_distributed.h"
#include "mpi_hello.h"

// Funzione wrapper per leggere l'array da ordinare
// in questo caso l'array viene generato casualmente a tempo di esecuzione
void read_input(int* out, int n) {
    array_random(out, n);
}

int main(int argc, char* argv[]) {

    // La dimensione dell'array viene letta in input
    int n = atoi(argv[1]); 

    MPI_Init(NULL, NULL);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    mpi_hello();

    int* arr = NULL;

    // Il processo 0 alloca memoria e legge un array di lunghezza n
    if (my_rank == 0) {
        arr = (int*)malloc(n * sizeof(int));
        read_input(arr, n);
    }

    // Qui incomincia la procedura di sorting, si inizia a prendere il tempo
    double start, finish;
    MPI_Barrier(MPI_COMM_WORLD); // Sincronizzo i processi
    start = MPI_Wtime();

    merge_sort_distributed(&arr, n);

    // Qui finisce la procedura di sorting, si finisce di prendere il tempo
    finish = MPI_Wtime();

    double elapsed = finish - start;

    if(my_rank == 0) printf("Elapsed time: %f\n", elapsed);

    // L'ultimo processo verifica la correttezza
    if (my_rank == 0) {
        int check = array_check_sorting(arr, n);
        printf("Check: %d\n", check);
    }

    if(my_rank == 0) free(arr);

    MPI_Finalize();

    return 0;

}