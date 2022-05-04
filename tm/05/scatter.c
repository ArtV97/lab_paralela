#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"


#define ARGS_ERROR -4
#define FOPEN_ERROR -3
#define N_ERROR -2
#define FILE_PARSE_ERROR -1
#define SUCCESS 1


void help() {
    fprintf(stderr, "### Possible Arguments ###\n");
    fprintf(stderr, "-f <filename>: file that contains X, N and the set S.\n");
    fprintf(stderr, "-n <N>: size of the set S.\n");
    fprintf(stderr, "-x <X>: value(int) to be searched in set S. # default value = 0\n\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "\t1) mpiexec -n 4 executable -f fin.txt\n");
    fprintf(stderr, "\t2) mpiexec -n 4 executable -n 30\n");
    fprintf(stderr, "\t3) mpiexec -n 4 executable -x 93 -n 30\n\n");
    fprintf(stderr, "Abort Codes:\n");
    fprintf(stderr, "\t1: Success\n");
    fprintf(stderr, "\t-1: File parse error, first line of the file should be: <x> <N>\n");
    fprintf(stderr, "\t-2: Expect number of items in S is different from N.\n");
    fprintf(stderr, "\t-3: File Open Error.\n");
    fprintf(stderr, "\t-4: Arguments error.\n");
    MPI_Abort(MPI_COMM_WORLD, ARGS_ERROR);
}


int main(int argc, char **argv) {
    int rank, size;
    MPI_Status status;

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int i; // used to iterate over arrays
    int N;
    int x;
    int subset_size_send;
    int subset_size_recv;
    int *S = NULL;

    if (rank == 0) {
        
        ////////////////////////////////////
        // Parse arguments
        ////////////////////////////////////

        x = 0;
        N = 0;
        char *filename = NULL;
        FILE *fin = NULL;
        int LINE_BUFFER = 64;
        char line[LINE_BUFFER+1];
        
        i = 1;
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

            i++;
        }

        // must have filename or x and N
        if (!filename && (N == 0)) help();
        else if (filename && (x != 0 || N != 0)) help();


        // Get x and N from file
        if (filename) {
            fin = fopen(filename, "r");
            if (!fin) {
                perror("Error");
                MPI_Abort(MPI_COMM_WORLD, FOPEN_ERROR);
            }

            // first line is x
            fgets(line, LINE_BUFFER, fin);
            
            char *tok = strtok(line, " ");
            if (!tok) {
                fprintf(stderr, "Error: First line of file must be: <x> <N>\n");
                MPI_Abort(MPI_COMM_WORLD, FILE_PARSE_ERROR);
            }
            x = atoi(tok);

            tok = strtok(NULL, " ");
            if (!tok) {
                fprintf(stderr, "Error: First line of file must be: <x> <N>\n");
                MPI_Abort(MPI_COMM_WORLD, FILE_PARSE_ERROR);
            }
            N = atoi(tok);
        }

        
        /////////////////////
        // Broadcast x and N
        /////////////////////
        
        printf("### Preparing Search...\nx = %d, N = %d\n", x, N);

        int input[] = {x, N};
        MPI_Barrier(MPI_COMM_WORLD);  // sync 1
        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);


        /////////////////////
        // Read or Generate S
        /////////////////////

        //int S[N]; // set
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
                    fprintf(stderr, "Error: File %s has more items than expected. Expected %d\n", filename, N);
                    fclose(fin);
                    MPI_Abort(MPI_COMM_WORLD, N_ERROR);
                }
                S[i] = atoi(line);
                i++;
            }
            fclose(fin);
            if (i != N) {
                fprintf(stderr, "Error: Expect number of items in set S is %d, got %d\n", N, i);
                MPI_Abort(MPI_COMM_WORLD, N_ERROR);
            }
        }
        else { // generate random S
            printf("Generating random S\n");
            srand(time(NULL));
            
            for (i = 0; i < N; i++) {
                S[i] = rand() % 1000;
            }
        }

        printf("S: ");
        for (i = 0; i < N; i++) {
            printf("%d ", S[i]);
        }
        printf("\n\n");


        ////////////////////
        // Declaring values
        ////////////////////

        subset_size_recv = N/(size); // number of elements for each process
        subset_size_send = subset_size_recv;

    }
    else {
        ////////////////////
        // Receiving x and N
        ////////////////////

        int input[2];

        MPI_Barrier(MPI_COMM_WORLD); // sync 1
        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);
        
        x = input[0];
        N = input[1];



        ////////////////////
        // Declaring values
        ////////////////////

        subset_size_recv = N/(size); // number of elements for each process
        subset_size_send = 0;
    }



    //////////////////////////////
    // Receiving/Sending subset Si
    //////////////////////////////

    int Si[subset_size_recv];
    MPI_Barrier(MPI_COMM_WORLD);  // sync 2
    MPI_Scatter(S, subset_size_send, MPI_INT, Si, subset_size_recv, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process %d subset: ", rank);
    for (i = 0; i < subset_size_recv; i++) {
    printf("%d ", Si[i]);
    }
    printf("\n");

    ///////////////////////////
    // Searching x in subset Si    
    ///////////////////////////

    
    //MPI_Barrier(MPI_COMM_WORLD);  // sync 3
    for (i = 0; i < subset_size_recv; i++) {
        if (Si[i] == x) {
            printf("%d found by process %d in position %d\n", x, rank, i);
            MPI_Abort(MPI_COMM_WORLD, SUCCESS);
            return 0;
        }
    }

    if (S) {
        free(S);
    }
    MPI_Finalize();
    return 0;
}