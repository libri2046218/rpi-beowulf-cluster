#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "merge_sort.h"
#include "array.h"

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

// Può essere chiamata solo dopo che è stato già chiamato MPI_Init(.)
void merge_sort_distributed(int** arrPtr, int n) {
    int* arr = *arrPtr;

    int comm_sz;
    int my_rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); // Salva il numero di processi all'interno del communicator in comm_sz
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Salva il rank del processo in my_rank

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

    // Il vettore locale viene ordinato 
    merge_sort(local_arr, 0, local_n[my_rank] - 1);

    // Inizializzazione di variabili ausiliarie per il cambio di comunicatore
    int new_comm_sz = comm_sz;
    int new_rank = my_rank;
    int color;
    MPI_Comm new_comm = MPI_COMM_WORLD;
    
    while(new_comm_sz > 1) {

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

    if(my_rank == 0) *arrPtr = local_arr;
}