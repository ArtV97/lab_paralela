#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"


#define ARGS_ERROR -3
#define FOPEN_ERROR -2
#define N_ERROR -1
#define SUCCESS 1


void read_from_f(char *filename, int *x, int *N) {
    return;
}

void help() {
    printf("### Possible Arguments ###\n");
    printf("\t-n <N>: size of the set S # this argument is obrigatory\n");
    printf("\t-f <filename>: file that contains X and set S\n");
    printf("\t-x <X>: value(int) to be searched in set S\n");
    MPI_Abort(MPI_COMM_WORLD, ARGS_ERROR);
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Status status;

    MPI_Init(NULL, NULL);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        ////////////////////////////////////
        // Parse arguments
        ////////////////////////////////////

        int x = 0;
        int N = 0;
        char *filename = NULL;
        FILE *fin;
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

            i++;
        }

        if (N == 0) help(); // must have argument N
        else if (!filename && x == 0) help(); // must have filename or x
        else if (filename && x != 0) help(); // can't have filename and x

        if (filename) {
            fin = fopen(filename, "r");
            if (!fin) {
                fprintf(stderr, "Error: Couldn't open file %s.\n", filename);
                MPI_Abort(MPI_COMM_WORLD, FOPEN_ERROR);
            }

            // first line is x
            fgets(line, LINE_BUFFER, fin);
            x = atoi(line);
        }

        /////////////////////
        // Broadcast x and N
        /////////////////////

        int input[] = {x, N};
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);


        ///////////////////////////////
        // Read or Generate S then Send
        ///////////////////////////////

        int S[N]; // set
        if (fin) { // read S from file
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
            srand(time(NULL));
            
            for (int i = 0; i < N; i++) {
                S[i] = rand();
            }
        }

        fprintf(stderr, "S: ");
        for (int i = 0; i < N; i++) {
            fprintf(stderr, "%d ", S[i]);
        }
        fprintf(stderr, "\n");
        
        int rest = N % (size-1);
        int subset_size = N/(size-1); // number of elements for each process

        MPI_Barrier(MPI_COMM_WORLD);
        for (int i = 0; i < size-1; i++) {
            if (i != size -1) {
                MPI_Send(S+(subset_size*i), subset_size, MPI_INT, i+1, 0, MPI_COMM_WORLD);
            }
            else { // last process will get the "rest" if any
                MPI_Send(S+(subset_size*i), subset_size + rest, MPI_INT, i+1, 0, MPI_COMM_WORLD);
            }
        }


        /////////////////////////
        // Waiting for the result
        /////////////////////////

        int result;
        for (int i = 0; i < size-1; i++) {
            MPI_Recv(&result, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG) { // found x in set S
                result = result + (subset_size*status.MPI_SOURCE);
                printf("%d found by process %d in position %d\n", x, status.MPI_SOURCE, result);
                MPI_Abort(MPI_COMM_WORLD, SUCCESS);
            }
        }
        printf("%d was not found in set S.\n", x);
    }
    else {
        ////////////////////
        // Receiving x and N
        ////////////////////

        int input[2];
        int x;
        int N;

        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);
        
        x = input[0];
        N = input[1];


        //////////////////////
        // Receiving subset Si
        //////////////////////
        
        int rest = N % (size-1);
        int subset_size = N/(size-1); // number of elements for each process
        if (rank == size-1) {
            subset_size = subset_size + rest;
        }

        int Si[subset_size];
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Recv(Si, subset_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        fprintf(stderr, "Process %d Si: ", rank);
        for (int i = 0; i < subset_size; i++) {
            fprintf(stderr, "%d ", Si[i]);
        }
        fprintf(stderr, "\n");
        ///////////////////////////
        // Searching x in subset Si    
        ///////////////////////////
        
        for (int i = 0; i < subset_size; i++) {
            if (Si[i] == x) {
                MPI_Send(&i, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
                MPI_Finalize();
                return 0;
            }
        }

        // x not found in this subset
        MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}