

/*
Um conjunto de threads são definidas para dividirem a carga do cálculo de pi. 
Para tal:

    uma variável escalar local que armazena a soma parcial, para cada thread
    depois uma variável compartilhada é atualizada com a soma do valor da variável local multiplicado pelo step

utilizando o pragma critical versão 1
*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

//static long num_steps = 100000;    
double step;

int main (int argc, char **argv) {
    if (argc != 3) {
        printf("Must pass argument <NUM_THREADS> <NUM_STEPS>\n");
        return 1;
    }

    int NUM_THREADS = atoi(argv[1]);
    long num_steps = atoi(argv[2]);

    double pi;
    step = 1.0/(double) num_steps;
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel
    {
        int i, id, nthrds; 
        double x, sum;

        id = omp_get_thread_num();
        nthrds = omp_get_num_threads();  
        for (i=id, sum=0.0;  i<num_steps; i = i+nthrds) {
            x = (i+0.5)*step;
            sum += 4.0/(1.0+x*x);
        }
        
        #pragma omp critical
        //#pragma omp atomic
           pi += sum * step;
    }

    printf("pi = %lf\n", pi);
    return 0;
}
