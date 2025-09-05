#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "merge_sort.h"
#include "array.h"

#define TIMING 1

// Funzione wrapper per leggere l'array da ordinare
// in questo caso l'array viene generato casualmente a tempo di esecuzione
void read_input(int* out, int n) {
    array_random(out, n);
}

int main(int argc, char* argv[]) {

    // La dimensione dell'array viene letta in input
    int n = atoi(argv[1]); 

    int comm_sz;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

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

    // Distribuzione equa del carico
    int local_n[comm_sz];
    for (int i = 0; i < comm_sz; i++) {
        local_n[i] = n / comm_sz;
    }

    // Distribuzione equa della parte non divisibile
    int remainder = n - (n/comm_sz)*comm_sz;
    for (int i = 0; i < remainder; i++){
        local_n[i] += 1;
    }

    // Inizializzazione del vettore che contiene l'informazione di displacement
    int displs[comm_sz];
    displs[0] = 0;
    for (int i = 1; i < comm_sz; i++){
        displs[i] = displs[i-1] + local_n[i-1];
    }

    // Allocazione di memoria dinamica per il vettore locale da ordinare
    int* local_arr = (int*)malloc(local_n[my_rank] * sizeof(int));

    // Il processo 0 invia il vettore agli altri processi che ricevono solo una parte del vettore originale
    // con numero di elementi definito da local_n
    MPI_Scatterv(arr, local_n, displs, MPI_INT, local_arr, local_n[my_rank], MPI_INT, 0, MPI_COMM_WORLD);


    // Il processo 0 libera la memoria del vettore di input
    if(my_rank == 0) {
        free(arr);
    }

    //printf("Process %d: ", my_rank);
    //array_print(local_arr, local_n[my_rank]);

    // Il vettore locale viene ordinato 
    merge_sort(local_arr, 0, local_n[my_rank] - 1);

    //printf("Process %d: ", my_rank);
    //array_print(local_arr, local_n[my_rank]);


    // Inizializzazione di variabili ausiliarie per il cambio di comunicatore
    int new_comm_sz = comm_sz;
    int new_rank = my_rank;
    int color;
    MPI_Comm new_comm = MPI_COMM_WORLD;
    
    while(new_comm_sz > 1) {

        //MPI_Barrier(new_comm);

        // I processi con rank dispari inviano il loro vettore locale
        if (new_rank % 2 == 1) {

            MPI_Send(local_arr, local_n[new_rank], MPI_INT, new_rank - 1, 0, new_comm);

            // Il processo non verrà incluso nel nuovo communicator
            color = MPI_UNDEFINED;

        } 
        // I processi con rank pari allocano spazio sia per il loro vettore che per quello che ricevono
        // e chiamano la procedura di merge per ordinare il nuovo vettore, sapendo che i due vettori sono già ordinati
        else if (new_rank % 2 == 0 && new_rank + 1 != new_comm_sz) {

            int* new_local_arr = (int*)malloc((local_n[new_rank] + local_n[new_rank + 1]) * sizeof(int));

            MPI_Recv(new_local_arr, local_n[new_rank + 1], MPI_INT, new_rank + 1, 0, new_comm, NULL);

            array_copy(local_arr, new_local_arr + local_n[new_rank + 1], local_n[new_rank]);

            merge(new_local_arr, 0, local_n[new_rank + 1] - 1, local_n[new_rank] + local_n[new_rank + 1] - 1);

            //array_print(new_local_arr, local_n[new_rank] + local_n[new_rank + 1]);

            //Deallocazione della memoria non più utilizzata
            free(local_arr);
            local_arr = new_local_arr;

            // Il processo farà parte del nuovo communicator
            color = 1;

        } else {
            // Il processo farà parte del nuovo communicator
            color = 1;
        }

        // Il vettore che mantiene in memoria l'informazione del carico per ogni nodo viene aggiornato
        array_reduce(local_n, new_comm_sz);
        
        // Viene creato un nuovo comunicatore
        MPI_Comm old_comm = new_comm;
        MPI_Comm_split(old_comm, color, new_rank, &new_comm);

        // Viene liberato lo spazio del vecchio comunicatore
        if (old_comm != MPI_COMM_WORLD) MPI_Comm_free(&old_comm);

        // I processi non presenti nel nuovo comunicatore escono dal ciclo
        if (color == MPI_UNDEFINED) break;

        // Assegnazione di nuovi rank
        MPI_Comm_rank(new_comm, &new_rank);
        MPI_Comm_size(new_comm, &new_comm_sz);
    }

    // Qui finisce la procedura di sorting, si finisce di prendere il tempo
    finish = MPI_Wtime();

    double elapsed = finish - start;

    if(my_rank == 0) printf("Elapsed time: %f \n", elapsed);

    // L'ultimo processo verifica la correttezza
    if (new_rank == 0) {
        int check = array_check_sorting(local_arr, n);
        printf("Check: %d\n", check);
    }

    free(local_arr);

    MPI_Finalize();

    return 0;

}