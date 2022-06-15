#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <omp.h>

void print_matrix ( int n_elements, int matrix[n_elements][n_elements]){
  for (int i=0; i < n_elements; ++i){
    printf("[");
    for(int j=0; j< n_elements; j++){
      if(j==n_elements-1) printf("%d", matrix[i][j]);
      else printf("%d, ", matrix[i][j]);
    }
    printf("]\n");
  }
  printf("\n");
}


int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size, find, n_elements;
  int infos[2];
  MPI_Status status;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (size < 2) {
    printf("Need at least 2 processes.\n");
    return 1;
  }

  if (argc < 2) {
    printf("Missing argument <n_elements>\n");
    return 1;
  }

  n_elements = atoi(argv[1]);


  if (rank == 0) {
    int matrix_a[n_elements][n_elements];
    int matrix_b[n_elements][n_elements];
    //int matrix_c[n_elements][n_elements];

    /********* gerando numeros aleatorios para preencher as matrizes  ********/
    srand(time(NULL));    
    for (int i=0; i < n_elements; ++i) {
      for(int j=0; j< n_elements; j++) {
        matrix_a[i][j] = rand() % 1000;
        matrix_b[i][j] = rand() % 1000;
      }
    }
    printf("Matriz A: \n");
    print_matrix(n_elements, matrix_a);    
    printf("Matriz B:\n");
    print_matrix(n_elements, matrix_b);

    int send_to = 0;
    for (int i = 0; i < n_elements; i++) {      
      send_to++;
      if (send_to == size) send_to = 1;

      // Send Row of Matrix A. TAG = row index
      // printf("Sending Matrix A row %d to %d\n", i, send_to);
      MPI_Send(matrix_a[i], n_elements, MPI_INT, send_to, i, MPI_COMM_WORLD);

      // Send Row of Matrix B. TAG = row index
      // printf("Sending Matrix B row %d to %d\n", i, send_to);
      MPI_Send(matrix_b[i], n_elements, MPI_INT, send_to, i, MPI_COMM_WORLD);
    }


    int **matrix_c = malloc(n_elements*sizeof(int*));
    for (int i = 0; i < n_elements; i++) {
      int *row = malloc(n_elements*sizeof(int));

      // Recv Row of Matrix C
      MPI_Recv(row, n_elements, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      matrix_c[status.MPI_TAG] = row;
    }

    printf("Matriz C:\n");
    //print_matrix(n_elements, matrix_c);

    for (int i = 0; i < n_elements; i++) {
      for (int j = 0; j < n_elements; j++) {
        printf("%d ", matrix_c[i][j]);
      }
      printf("\n");
      free(matrix_c[i]);
    }
    free(matrix_c);
  }
  
  else {
    int row_a[n_elements];
    int row_b[n_elements];
    int row_c[n_elements];

    while (1) {
      // Recv Row of Matrix A
      MPI_Recv(row_a, n_elements, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      // Recv Row of Matrix B
      MPI_Recv(row_b, n_elements, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      int num_threads = 2+2*(n_elements / 10000);
      omp_set_num_threads(num_threads);

      #pragma omp parallel for
      for (int i = 0; i < n_elements; i++) {
        row_c[i] = row_a[i] + row_b[i];
      }

      // Send row of Matrix C .TAG = row index
      MPI_Send(row_c, n_elements, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD);
      
      if (n_elements - status.MPI_TAG <= rank) break;
    }
  }

  MPI_Finalize();
  return 0;
}
