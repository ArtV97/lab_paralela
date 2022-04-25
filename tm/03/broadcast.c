#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define N_ELEMENTS_ERROR 20
#define BUFFER_OVERFLOW_ERROR 21
#define DATA_ERROR 22

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  double sum = 0.0;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size != 3) {
    printf("Must have 3 processes!\n");
    MPI_Finalize();
    exit(1);
  }

  if (rank == 0) {
    int line_buffer = 4096;
    char line[line_buffer+1];
    
    FILE *fin;
    fin = fopen("input.txt", "r");
    
    fgets(line, line_buffer, fin); // first line is n_elements
    int n_elements = atoi(line);
    if (n_elements == 0) {
      printf("n_elements(first line of file) must be greater than 0!\n");
      MPI_Abort(MPI_COMM_WORLD, N_ELEMENTS_ERROR);
    }

    /********* reading the number of elements in n_elements  ********/
    double data[n_elements];

    int index = -1;
    while (fgets(line, line_buffer, fin) != NULL) {
      char *tok = strtok(line, "\n");
      char *aux;

      while (tok) {
        index++;
        if (index +1 > n_elements) {
          printf("data Overflow... data size = %ld.\n", n_elements*sizeof(double));
          MPI_Abort(MPI_COMM_WORLD, BUFFER_OVERFLOW_ERROR);
        }
        
        /*** storing each element in data[i]   *****/
        data[index] = strtod(tok, &aux);
        sum = sum + data[index];

        tok = strtok(NULL, "\n");
      }
    }


    if (index + 1 < n_elements) {
      printf("Missing elements: got %d expected %d.\n", index+1, n_elements);
      MPI_Abort(MPI_COMM_WORLD, DATA_ERROR);
    }


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&n_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);


    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(data, n_elements, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  }
  else {
    int n = 0; // number of elements

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast (&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (n == 0) {
      printf("Error Process %d: data size = 0.\n", rank);
      MPI_Finalize();
      exit(1);
    }
    
    double data[n];
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast (data, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (int i = 0; i < n; i++) {
      if (rank == 1) {
        if (data[i] >= 0.0) sum = sum + data[i];
      }
      else {
        if (data[i] < 0.0) sum = sum + data[i];
      }
    }
  }

 
  printf("Process %d\n\tSum = %lf\n", rank, sum);
  MPI_Finalize();

  return 0;
}