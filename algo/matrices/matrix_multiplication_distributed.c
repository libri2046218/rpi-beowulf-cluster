#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include "matrix.h"
#include "array.h"

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

    Matrix* A;
    Matrix* B;

    // Il processo 0 alloca memoria e legge le due matrici da moltiplicare
    if(my_rank == 0) {
        A = matrix_alloc(n);
        read_input(A);

        B = matrix_alloc(n);
        read_input(B);

        //matrix_print(A);
        //matrix_print(B);
    }

    // Qui incomincia la procedura, si inizia a prendere il tempo
    double start, finish;
    MPI_Barrier(MPI_COMM_WORLD); // Sincronizzo i processi
    start = MPI_Wtime();

    // Distribuzione equa del carico
    int local_n[comm_sz];
    for (int i = 0; i < comm_sz; i++) {
        local_n[i] = (n / comm_sz) * n;
    }

    // Distribuzione equa della parte non divisibile
    int remainder = n - (n/comm_sz)*comm_sz;
    for (int i = 0; i < remainder; i++){
        local_n[i] += n;
    }

    // Inizializzazione del vettore che contiene l'informazione di displacement
    int displs[comm_sz];
    displs[0] = 0;
    for (int i = 1; i < comm_sz; i++){
        displs[i] = displs[i-1] + local_n[i-1];
    }

    // Allocazione dei buffer dei vettori da moltiplicare (più vettori sono allocati in modo contiguo)
    int* local_a_buf = (int*)malloc(local_n[my_rank] * sizeof(int));
    int* local_b_buf = (int*)malloc(local_n[my_rank] * sizeof(int));

    // Vettore delle posizioni dei vettori nel buffer
    int* local_a_rows[local_n[my_rank] / n];
    int* local_b_rows[local_n[my_rank] / n];
    for(int i = 0; i < local_n[my_rank] / n; i++){
        local_a_rows[i] = local_a_buf + n * i;
        local_b_rows[i] = local_b_buf + n * i;
    }

    // L'obiettivo è che ogni nodo calcoli il prodotto tra una (o più) colonna di A con una riga di B
    // formando una matrice n * n, trasponiamo la matrice A così che i valori delle colonne di A siano
    // salvati in memoria in modo contiguo
    if (my_rank == 0) matrix_transpose(A);

    // Il processo 0 invia i vettori che rappresentano le matrici (righe collocate in modo contiguo) agli altri processi
    // che ricevono una o più righe della stessa matrice 
    if(my_rank == 0){
        MPI_Scatterv(A->M, local_n, displs, MPI_INT, local_a_buf, local_n[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(B->M, local_n, displs, MPI_INT, local_b_buf, local_n[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
    } else {
        MPI_Scatterv(NULL, NULL, NULL, NULL, local_a_buf, local_n[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Scatterv(NULL, NULL, NULL, NULL, local_b_buf, local_n[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
    }

    // Allocazione di memoria della sottomatrice C dove ogni processo salva i risultati
    Matrix* sub_C = matrix_alloc(n);
    matrix_empty(sub_C);

    //Per ogni coppia di vettori delle matrici A e B viene calcolato il prodotto 
    //tra la colonna di A e la riga di B e viene aggiunto il risultato alla matrice sub_C
    for (int row = 0; row < local_n[my_rank] / n; row++){
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < n; j++) {
                int sub_c_val;
                matrix_get(sub_C, i, j, &sub_c_val);
                int val = local_a_rows[row][i] * local_b_rows[row][j] + sub_c_val;
                matrix_set(sub_C, i, j, val);
            }
        }
    }

    //matrix_print(sub_C);
    
    Matrix* C;
    // Il processo 0 alloca memoria per la matrice risultato
    if(my_rank == 0) C = matrix_alloc(n);

    // Ogni processo invia la sua matrice (sottoforma di array n*n) e viene effettuata
    // la somma per riduzione 
    if(my_rank == 0) MPI_Reduce(sub_C->M, C->M, n * n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    else MPI_Reduce(sub_C->M, NULL, n * n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Qui finisce la procedura, si finisce di prendere il tempo
    finish = MPI_Wtime();

    //if(my_rank == 0) matrix_print(C);

    double elapsed = finish - start;

    if(my_rank == 0) printf("Elapsed time: %f \n", elapsed);
    
    MPI_Finalize();
    if(my_rank == 0) {
        matrix_free(A);
        matrix_free(B);
    }
}