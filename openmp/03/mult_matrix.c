#include <stdio.h>
#include <stdlib.h>
//#include <omp.h>

#define SIZE 1000
//int A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
int **A;
int **B;
int **C;


void fill_matrix(int **m, int n) {
	m = (int **)malloc(n*sizeof(int *));
	for (int i = 0; i < n; i++) {
		m[i] = (int *)malloc(n*sizeof(int));
		for (int j = 0; j < n; j++) {
			m[i][j] = i * j;
		}
	}
}

void free_matrix(int **m, int n) {
	for (int i = 0; i < n; i++) {
		free(m);
	}

	free(m);
}

int main(int argc, char **argv) {
	if (argc != 2) return 1;

	int i,j,k,N;
	N = atoi(argv[1]);
	fill_matrix(A, N);
	fill_matrix(B, N);

	printf("%d\n", A[2][2]);
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			C[i][j] = 0;
			for (k = 0; k < SIZE; k++) {
				C[i][j] = C[i][j] + A[i][j] * B[i][j];
			}
		}
	}

	free_matrix(A, N);
	free_matrix(B, N);
	free_matrix(C, N);

	return 0;
}