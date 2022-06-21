#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


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
    int N, M;

    if (argc != 3) {
        printf("Missing parameters: \n\tN: number of rows\n\tM: number of columns\n");
        return 1;
    }

    N = atoi(argv[1]);
    M = atoi(argv[2]);

    int **matrix_a = malloc(N*sizeof(int*));
    int **matrix_b = malloc(N*sizeof(int*));
    int **matrix_c = malloc(N*sizeof(int*));


    /********* gerando numeros aleatorios para preencher as matrizes  ********/
    srand(time(NULL));

    for (int i = 0; i < N; ++i) {
        matrix_a[i] = malloc(M*sizeof(int));
        matrix_b[i] = malloc(M*sizeof(int));
        matrix_c[i] = malloc(M*sizeof(int));

        for(int j = 0; j < M; j++) {
            matrix_a[i][j] = rand() % 1000;
            matrix_b[i][j] = rand() % 1000;
        }
    }

    printf("Matriz A: \n");
    print_matrix(matrix_a, N, M);
    printf("Matriz B:\n");
    print_matrix(matrix_b, N, M);

    int num_threads = 2+2*(N*M / 20000);
    omp_set_num_threads(num_threads);

    #pragma parallel for
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            matrix_c[i][j] = matrix_a[i][j] + matrix_b[i][j];
        }
    }

    printf("Matriz C:\n");
    print_matrix(matrix_c, N, M);
    
    free_matrix(matrix_a, N);
    free_matrix(matrix_b, N);
    free_matrix(matrix_c, N);
  
    return 0;
}
