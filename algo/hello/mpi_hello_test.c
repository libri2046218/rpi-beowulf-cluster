#include "mpi_hello.h"
#include <mpi.h>
int main(){
    MPI_Init(NULL, NULL);
    mpi_hello();
    MPI_Finalize();
    return 0;
}