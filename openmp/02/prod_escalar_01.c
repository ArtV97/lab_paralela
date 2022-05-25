#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Missing argument <array size>\n");
        return 1;
    }
    int n = atoi(argv[1]);
    double sum = 0;
    double a[n], b[n];
    int i;
    for (i = 0; i < n; i++){
            a[i] = i * 0.5;
            b[i] = i * 2.0;
    }
    sum = 0;

    clock_t initial = clock();
    for (i=1; i<n; i++){
        sum = sum + a[i]*b[i];
    }
    clock_t final = clock();
    double result = ((double)(final-initial)/CLOCKS_PER_SEC);
    printf ("sum = %f\n", sum);
    printf("Calculation Execution Time: %lf\n", result);

    return 0;
}