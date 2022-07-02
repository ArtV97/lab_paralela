#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpi.h"


#define ARGS_ERROR -4
#define FOPEN_ERROR -3
#define N_ERROR -2
#define FILE_PARSE_ERROR -1


void help() {
    fprintf(stderr, "### Possible Arguments ###\n");
    fprintf(stderr, "-f <filename>: file that contains X, N and the set S.\n");
    fprintf(stderr, "-n <N>: size of the set S.\n");
    fprintf(stderr, "-x <X>: value(int) to be searched in set S.\n\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "\t1) mpiexec -n 4 executable -f fin.txt\n");
    fprintf(stderr, "\t2) mpiexec -n 4 executable -x 93 -n 30\n\n");
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
    int found = 0;
    MPI_Request request;
    int flag = 0;


    if (rank == 0) {
        ////////////////////////////////////
        // Parse arguments
        ////////////////////////////////////

        int x = -1;
        int N = 0;
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
        if (!filename && (N == 0 || x == -1)) help();
        else if (filename && (x != -1 || N != 0)) help();


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
        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);


        /////////////////////
        // Read or Generate S
        /////////////////////

        int S[N]; // set
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

        // printf("S: ");
        // for (i = 0; i < N; i++) {
        //     printf("%d ", S[i]);
        // }
        // printf("\n\n");


        ////////////////////
        // Sending the set S
        ////////////////////
        double initial = MPI_Wtime();

        printf("### Running No-Scatter Search...\n");
        int rest = N % (size);
        int subset_size = N/(size); // number of elements for each process

        //for (i = 0; i < size-1; i++) {
            //int dst_rank = i+1;
        for (i = 1; i < size; i++) {
            int dst_rank = i;
            if (dst_rank != size -1) {
                MPI_Send(S+(subset_size*i), subset_size, MPI_INT, dst_rank, 0, MPI_COMM_WORLD);
            }
            else { // last process will get the "rest" if any
                MPI_Send(S+(subset_size*i), subset_size + rest, MPI_INT, dst_rank, 0, MPI_COMM_WORLD);
            }
        }


        ///////////////////////////
        // Searching x in subset Si
        ///////////////////////////
        for (i = 0; i < subset_size; i++) {
            if (S[i] == x) { // process 0 found x
                printf("%d found by process %d in position %d\n", x, rank, i);
                found = 1;
                // broadcast that x was found!
                MPI_Bcast(&found, 1, MPI_INT, 0, MPI_COMM_WORLD);
            }
        }

        int finished_counter = 0;
        while (finished_counter != (size-1)) {
            MPI_Recv(&found, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (found) {
                MPI_Bcast(&found, 1, MPI_INT, 0, MPI_COMM_WORLD);
                break;
            }

            finished_counter++;
        }
        
        double final = MPI_Wtime();
        double result = final - initial;
        if (finished_counter == (size-1)) printf("%d not found in set S\n", x);
        printf("Calculation Execution Time: %lf\n", result);
    }
    else {
        ////////////////////
        // Receiving x and N
        ////////////////////

        int input[2];
        int x;
        int N;

        MPI_Bcast(input, 2, MPI_INT, 0, MPI_COMM_WORLD);
        
        x = input[0];
        N = input[1];


        //////////////////////
        // Receiving subset Si
        //////////////////////
        
        //int rest = N % (size-1);
        int subset_size = N/(size); // number of elements for each process
        if (rank == size-1) {
            int rest = N % (size);
            subset_size = subset_size + rest;
        }

        int Si[subset_size];
        MPI_Recv(Si, subset_size, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        // printf("Process %d subset: ", rank);
        // for (i = 0; i < subset_size; i++) {
        //     printf("%d ", Si[i]);
        // }
        // printf("\n");

        
        ///////////////////////////
        // Searching x in subset Si    
        ///////////////////////////
        for (i = 0; i < subset_size; i++) {
            // non-blocking broadcast.
            // broadcast is received when another process found x
            MPI_Ibcast(&found, 1, MPI_INT, 0, MPI_COMM_WORLD, &request);
            MPI_Test(&request, &flag, &status);
            if (flag) {
                break;
            }

            if (Si[i] == x) {
                printf("%d found by process %d in position %d\n", x, rank, i);
                found = 1;

                // tell process 0 that x was found
                MPI_Send(&found, 1, MPI_INT, 0, 0, MPI_COMM_WORLD); 
            }
        }

        // tell process 0 that this process didn't found x
        if (!found) MPI_Send(&found, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}