#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: compare_bcast num_elements num_trials\n");
    exit(1);
  }
  MPI_Init(&argc, &argv);

  int rank, size;
  double sum = 0.0;
  int n_elements = atoi(argv[1]);

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != 3) {
    printf("Must have 3 processes");
    exit(1);
  }

  if (rank == 0) {
    int line_buffer = 10000;
    char line[line_buffer+1];
    FILE *fin;
    /********* reading the number of elements in n_elements  ********/
    double buffer[n_elements];
    
    fin = fopen("input.txt", "r");
    int n = -1;
    while (fgets(line, line_buffer, fin) != NULL) {
      char *tok = strtok(line, " ");
      char *aux;

      while (tok) {
        n++;
        if (n +1 > n_elements) {
          printf("Buffer Overflow... Buffer size = %ld\n", n_elements*sizeof(double));
          exit(1);
        }
        
        /*** storing each element in buffer[i]   *****/
        buffer[n] = strtod(tok, &aux);
        sum = sum + buffer[n];

        tok = strtok(NULL, " ");
      }
    }

    //MPI_Barrier(MPI_COMM_WORLD);
    n++; // n + 1 elements
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);


    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(buffer, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  }
  else {
    int n = 0;

    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n == 0) {
      printf("Error: Buffer size = 0\n");
      exit(1);
    }
    
    double buffer[n];
    //MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast (buffer, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++) {
      if (rank == 1) {
        if (buffer[i] >= 0.0) sum = sum + buffer[i];
      }
      else {
        if (buffer[i] < 0.0) sum = sum + buffer[i];
      }
    }
  }

 
  printf("Process %d - Sum = %lf\n", rank, sum);
  MPI_Finalize();

  return 0;
}