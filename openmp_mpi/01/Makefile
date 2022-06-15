all: matrix_sum_hybrid.c
	-mkdir bin
	mpicc matrix_sum_hybrid.c -o bin/matrix_sum_hybrid -fopenmp

clean:
	rm -r bin
	rm *.o