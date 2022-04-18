#include <stdio.h>
#include "mpi.h"

int main( int argc, char **argv ){
    int rank; // process rank "my rank"
    int size; // number of process
    int target; // target of the comunication, MPI_Send destination and MPI_Recv source
    int value; // variable to store MPI_Recv content
    MPI_Status status;

    MPI_Init( &argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    if(size % 2 != 0){
        printf("Numero de processos deve ser par\n");
        MPI_Finalize();
        return 0;
    } 

    if(rank < size/2){
        target = size/2 + rank;
    }else{
        target = rank - size/2;
    }
    
    // printf("my rank is %d, %d, %d\n", rank, target, target);
    MPI_Send(&rank, 1, MPI_INT, target , 0, MPI_COMM_WORLD);
    MPI_Recv (&value, 1, MPI_INT, target, 0, MPI_COMM_WORLD, &status);

    printf("Tarefa %d enviou seu id para %d", rank, target);
    if(rank == 0) printf(" ## Master process ##\n");
    else printf("\n");

    MPI_Finalize();
    return 0;
}