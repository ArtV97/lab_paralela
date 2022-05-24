#include <stdio.h>
#include <omp.h>

int main() {
	omp_set_num_threads(4);
	# pragma omp parallel num_threads(2)
	{
		int np = omp_get_num_threads();
		int iam = omp_get_thread_num();
		printf("Hello from thread %d (total %d)\n", iam, np);
	}

	printf("Fork 2\n");
	# pragma omp parallel
	{
		int np = omp_get_num_threads();
		int iam = omp_get_thread_num();
		printf("Hello from thread %d (total %d)\n", iam, np);
	}

	return 0;
}