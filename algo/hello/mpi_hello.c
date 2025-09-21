#include <mpi.h>
#include <stdio.h>

void mpi_hello() {

    // Ottieni il numero totale di processi
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Ottieni il rank del processo
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Ottieni il nome del processore
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Stampa messaggio da ogni processo
    printf("Hello from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

}

