#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "merge_sort.h"
#include "array.h"

void read_vector(
    int* local_arr,
    int* local_n,
    int* displs,
    int n,
    int my_rank,
    MPI_Comm comm){

        int* arr = NULL;

        if (my_rank == 0) {
            arr = malloc(n*sizeof(int));

            // Genera un array
            array_random(arr, n);

            array_print(arr, n);

            MPI_Scatterv(arr, local_n, displs, MPI_INT, local_arr, local_n[my_rank], MPI_INT, 0, comm);
            free(arr);
        } else {
            MPI_Scatterv(arr, local_n, displs, MPI_INT, local_arr, local_n[my_rank], MPI_INT, 0, comm);
        }

}

void distributed_merge(){

}



int main(int argc, char* argv[]) {

    int n = atoi(argv[1]); // Dimensione dell'array

    int comm_sz;
    int my_rank;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int local_n[comm_sz];
    for (int i = 0; i < comm_sz; i++) {
        local_n[i] = n / comm_sz;
    }

    // Distribuzione equa della parte non divisibile
    int remainder = n - (n/comm_sz)*comm_sz;
    for (int i = 0; i < remainder; i++){
        local_n[i] += 1;
    }

    int displs[comm_sz];
    displs[0] = 0;
    for (int i = 1; i < comm_sz; i++){
        displs[i] = displs[i-1] + local_n[i-1];
    }

    int* local_arr = (int*)malloc(local_n[my_rank] * sizeof(int));

    read_vector(local_arr, local_n, displs, n, my_rank, MPI_COMM_WORLD);

    
    printf("Process %d: ", my_rank);
    array_print(local_arr, local_n[my_rank]);

    merge_sort(local_arr, 0, local_n[my_rank] - 1);

    printf("Process %d: ", my_rank);
    array_print(local_arr, local_n[my_rank]);


    int new_comm_sz = comm_sz;
    int new_rank = my_rank;
    int color;
    MPI_Comm new_comm = MPI_COMM_WORLD;
    
    while(new_comm_sz > 1) {

        MPI_Barrier(new_comm);

        MPI_Comm_rank(new_comm, &new_rank);
        MPI_Comm_size(new_comm, &new_comm_sz);

        if (new_rank % 2 == 1) {

            MPI_Send(local_arr, local_n[new_rank], MPI_INT, new_rank - 1, 0, new_comm);

            color = MPI_UNDEFINED;

        } else if (new_rank % 2 == 0 && new_rank + 1 != new_comm_sz) {

            int* new_local_arr = (int*)malloc((local_n[new_rank] + local_n[new_rank + 1]) * sizeof(int));

            MPI_Recv(new_local_arr, local_n[new_rank + 1], MPI_INT, new_rank + 1, 0, new_comm, NULL);

            array_copy(local_arr, new_local_arr + local_n[new_rank + 1], local_n[new_rank]);

            merge(new_local_arr, 0, local_n[new_rank + 1] - 1, local_n[new_rank] + local_n[new_rank + 1] - 1);

            array_print(new_local_arr, local_n[new_rank] + local_n[new_rank + 1]);

            free(local_arr);
            local_arr = new_local_arr;

            color = 1;

        } else {
            color = 1;
        }

        array_reduce(local_n, new_comm_sz);
        
        MPI_Comm old_comm = new_comm;

        MPI_Comm_split(old_comm, color, new_rank, &new_comm);

        if (old_comm != MPI_COMM_WORLD) MPI_Comm_free(&old_comm);
        if (color == MPI_UNDEFINED) break;

        
    }

    MPI_Finalize();

    return 0;

}