#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("Missing arguments <array size> and <num_threads>\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    double sum = 0;
    double a[n], b[n];
    
    for (int i = 0; i < n; i++){
            a[i] = i * 0.5;
            b[i] = i * 2.0;
    }
    sum = 0;

    omp_set_num_threads(num_threads);
    clock_t initial = clock();
    # pragma omp parallel
    {
        int id, i, istart, iend;
        id = omp_get_thread_num();
        istart = id * n / num_threads;
        iend = (id+1) * n / num_threads;
        
        if (id == num_threads - 1) iend = n;

        for(i = istart; i < iend; i++) {
            # pragma omp atomic
            sum = sum + a[i]*b[i];
        }
    }
    clock_t final = clock();
    double result = ((double)(final-initial)/CLOCKS_PER_SEC);
    printf ("sum = %f\n", sum);
    printf("Calculation Execution Time: %lf\n", result);

    return 0;
}