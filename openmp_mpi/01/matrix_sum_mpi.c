#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>


///////////////
// free matrix
///////////////
void free_matrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) free(matrix[i]);
    free(matrix);
}

////////////////
// print matrix
////////////////
void print_matrix(int **matrix, int n, int m) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}





int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank, size, N, M;
    int infos[2];
    MPI_Status status;
    char *filename = NULL;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
    printf("Need at least 2 processes.\n");
    MPI_Finalize();
    return 1;
    }

    if (argc != 3) {
        printf("Missing parameters: \n\tN: number of rows\n\tM: number of columns\n");
        MPI_Finalize();
        return 1;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    if (rank == 0) {
        int **matrix_a = malloc(N*sizeof(int*));
        int **matrix_b = malloc(N*sizeof(int*));
        int **matrix_c;

        /********* gerando numeros aleatorios para preencher as matrizes  ********/
        srand(time(NULL));

        for (int i = 0; i < N; ++i) {
            matrix_a[i] = malloc(M*sizeof(int));
            matrix_b[i] = malloc(M*sizeof(int));

            for(int j = 0; j < M; j++) {
            matrix_a[i][j] = rand() % 1000;
            matrix_b[i][j] = rand() % 1000;
            }
        }

        printf("Matriz A: \n");
        print_matrix(matrix_a, N, M);
        printf("Matriz B:\n");
        print_matrix(matrix_b, N, M);

        int send_to = 0;
        for (int i = 0; i < N; i++) {
            send_to++;
            if (send_to == size) send_to = 1;

            // Send Row of Matrix A. TAG = row index
            // printf("Sending Matrix A row %d to %d\n", i, send_to);
            MPI_Send(matrix_a[i], M, MPI_INT, send_to, i, MPI_COMM_WORLD);

            // Send Row of Matrix B. TAG = row index
            // printf("Sending Matrix B row %d to %d\n", i, send_to);
            MPI_Send(matrix_b[i], M, MPI_INT, send_to, i, MPI_COMM_WORLD);
        }


        matrix_c = malloc(N*sizeof(int*));
        for (int i = 0; i < N; i++) {
            int *row = malloc(M*sizeof(int));

            // Recv Row of Matrix C
            MPI_Recv(row, M, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            matrix_c[status.MPI_TAG] = row;
        }

        printf("Matriz C:\n");
        print_matrix(matrix_c, N, M);

        free_matrix(matrix_a, N);
        free_matrix(matrix_b, N);
        free_matrix(matrix_c, N);
    }

    else {
        int row_a[M];
        int row_b[M];
        int row_c[M];

        int stop_at_row;
        for (int k = (rank-1); k < N; stop_at_row = k, k = k + (size-1));
        //printf("rank %d, stop at: %d\n", rank, stop_at_row);

        while (1) {
            // Recv Row of Matrix A
            MPI_Recv(row_a, M, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Recv Row of Matrix B
            MPI_Recv(row_b, M, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            for (int i = 0; i < M; i++) {
                row_c[i] = row_a[i] + row_b[i];
            }

            // Send row of Matrix C .TAG = row index
            MPI_Send(row_c, M, MPI_INT, 0, status.MPI_TAG, MPI_COMM_WORLD);
            
            if (status.MPI_TAG == stop_at_row) break;
        }
    }

    MPI_Finalize();
    return 0;
}
