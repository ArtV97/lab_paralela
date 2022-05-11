
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"

#define buffer_count 100

int main( argc, argv )
int argc;
char **argv;
{   int rank,  size;
    MPI_Status status;
    //MPI_Request req;
    int buffer[buffer_count];
    
    
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    // This file is provided as an example of blocking scenario. 

    // Initialising buffer :
    for (int i=0; i < buffer_count; ++i)
        buffer[i] = (rank == 0 ? i*2 : 0);

    MPI_Barrier(MPI_COMM_WORLD);
    // Starting the chronometer
    double time = -MPI_Wtime(); // This command helps us measuring time. We will see more about it later !
    // Simulate working
    if (rank == 0) {
    	sleep(3);
    	MPI_Send(buffer, buffer_count, MPI_INT, 1, 0, MPI_COMM_WORLD);
    	sleep(6);
    
    	// Modifying the buffer for second step
    	for (int i=0; i < buffer_count; ++i)
      		buffer[i] = -i;
    
    	MPI_Send(buffer, buffer_count, MPI_INT, 1, 1, MPI_COMM_WORLD);
    }
    else {
    	sleep(5);
    	MPI_Recv(buffer, buffer_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    	sleep(3);
    	MPI_Recv(buffer, buffer_count, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    time += MPI_Wtime();
  
  // This line gives us the maximum time elapsed on each process.
  // We will see about reduction later on !
    double final_time;
    MPI_Reduce(&time, &final_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    printf (" Process %d finalizing \n", rank);
    
    if (rank == 0) printf("Total time for blocking scenario :  %f sec\n", final_time);

    
    MPI_Finalize( );
    
    return(0);
}
