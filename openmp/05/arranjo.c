#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


void help() {
    fprintf(stderr, "### Possible Arguments ###\n");
	fprintf(stderr, "-num_threads <num_threads>: number of threads to be created.\n");
    fprintf(stderr, "-f <filename>: file that contains X, N and the set S.\n");
    fprintf(stderr, "-n <N>: size of the set S.\n");
    fprintf(stderr, "-x <X>: value(int) to be searched in set S. # default value = 0\n\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "\t1) ./executable  -num_threads 4 -f fin.txt\n");
    fprintf(stderr, "\t2) ./executable -num_threads 4 -n 30\n");
    fprintf(stderr, "\t3) ./executable -num_threads 3 -x 93 -n 30\n\n");
	exit(1);
}

int main(int argc, char **argv) {
	////////////////////////////////////
	// Parse arguments
	////////////////////////////////////

	int x = 0;
	int N = 0;
	int num_threads = 0;
	int *S;
	char *filename = NULL;
	FILE *fin = NULL;
	int LINE_BUFFER = 64;
	char line[LINE_BUFFER+1];
	
	int i = 1;
	while (i+1 < argc) {
		if (strcmp(argv[i], "-f") == 0) {
			filename = argv[++i];
		}
		else if (strcmp(argv[i], "-x") == 0) {
			x = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-n") == 0){
			N = atoi(argv[++i]);
		}
		else if (strcmp(argv[i], "-num_threads") == 0){
			num_threads = atoi(argv[++i]);
		}

		i++;
	}

	// must have (filename and num_threads) or (x and N and num_threads)
	if (!num_threads) help();
	else if (!filename && (N == 0)) help();
	else if (filename && (x != 0 || N != 0)) help();


	// Get x and N from file
	if (filename) {
		fin = fopen(filename, "r");
		if (!fin) {
			perror("Error");
		}

		// first line is x
		fgets(line, LINE_BUFFER, fin);
		
		char *tok = strtok(line, " ");
		if (!tok) {
			printf("Error: First line of file must be: <x> <N>\n");
		}
		x = atoi(tok);

		tok = strtok(NULL, " ");
		if (!tok) {
			printf("Error: First line of file must be: <x> <N>\n");
		}
		N = atoi(tok);
	}

	/////////////////////
	// Read or Generate S
	/////////////////////

	S = (int*)malloc(N*sizeof(N));
	if (fin) { // read S from file
		printf("Reading S from %s\n", filename);
		i = 0;
		int n;
		while (fgets(line, LINE_BUFFER, fin)) {
			n = strlen(line);
			if (line[--n] == '\n') line[n] = '\0'; // remove '\n'
			if (line[--n] == '\r') line[n] = '\0'; // remove '\r'

			if (!(i < N)) {
				printf("Error: File %s has more items than expected. Expected %d\n", filename, N);
				fclose(fin);
			}
			S[i] = atoi(line);
			i++;
		}
		fclose(fin);
		if (i != N) {
			printf("Error: Expect number of items in set S is %d, got %d\n", N, i);
		}
	}
	else { // generate random S
		printf("Generating random S\n");
		srand(time(NULL));
		
		for (i = 0; i < N; i++) {
			S[i] = rand() % 1000;
		}
	}

	// printf("S: ");
	// for (i = 0; i < N; i++) {
	// 	printf("%d ", S[i]);
	// }
	// printf("\n\n");


	int end_flag = 0;
	omp_set_num_threads(num_threads);
	
	clock_t initial = clock();
	#pragma omp parallel for
	for (int i = 0; i < N; i++) {
		if (end_flag) continue;
		if (S[i] == x) {
			int previous = end_flag;
			#pragma omp critical
			{
				end_flag = 1;
			}

			if (!previous) {
				int curr_thread = omp_get_thread_num();
				printf("%d found by thread %d in position %d.\n", x, curr_thread, i);
			}			
		}
	}

	if (!end_flag) {
		printf("%d not found.\n", x);
	}

    clock_t final = clock();
    double result = ((double)(final-initial)/CLOCKS_PER_SEC);
	printf("Calculation Execution Time: %lf\n", result);

	return 0;
}