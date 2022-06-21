all: matrix_sum_hybrid.c matrix_sum_openmp.c matrix_sum_mpi.c
	-mkdir bin
	mpicc matrix_sum_hybrid.c -o bin/matrix_sum_hybrid -fopenmp
	gcc matrix_sum_openmp.c -o bin/matrix_sum_openmp -fopenmp
	mpicc matrix_sum_mpi.c -o bin/matrix_sum_mpi

clean:
	rm -r bin
	rm *.o