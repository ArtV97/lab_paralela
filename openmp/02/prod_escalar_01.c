#include <stdlib.h>
#include <stdio.h>

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

    for (i=1; i<n; i++){
        sum = sum + a[i]*b[i];
    }
    printf ("sum = %f\n", sum);

    return 0;
}