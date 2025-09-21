#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "matrix.h"
#include "array.h"
#include "matrix_multiplication_distributed.h"
#include "mpi_hello.h"

void read_input(Matrix* out){
    matrix_random(out);
}

int main(int argc, char* argv[]){

    srand(time(NULL));

    // Dimensione della matrice letta in input
    int n = atoi(argv[1]);

    int comm_sz;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    mpi_hello();

    Matrix* A;
    Matrix* B;
    Matrix* C;

    // Il processo 0 alloca memoria e legge le due matrici da moltiplicare e la matrice risultato
    if(my_rank == 0) {
        A = matrix_alloc(n);
        read_input(A);

        B = matrix_alloc(n);
        read_input(B);

        C = matrix_alloc(n);
        matrix_empty(C);
    }

    // Qui incomincia la procedura, si inizia a prendere il tempo
    double start, finish;
    MPI_Barrier(MPI_COMM_WORLD); // Sincronizzo i processi
    start = MPI_Wtime();

    matrix_multiplication_distributed(A, B, C, n);

    // Qui finisce la procedura, si finisce di prendere il tempo
    finish = MPI_Wtime();

    double elapsed = finish - start;

    if(my_rank == 0) printf("Elapsed time: %f \n", elapsed);
    
    MPI_Finalize();
    if(my_rank == 0) {
        matrix_free(A);
        matrix_free(B);
    }
}