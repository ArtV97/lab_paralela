all: arranjo_seq.c arranjo_openmp.c arranjo_scatter.c arranjo_no_scatter.c
	-mkdir bin
	gcc arranjo_seq.c -o bin/arranjo_seq
	gcc arranjo_openmp.c -o bin/arranjo_openmp -fopenmp
	mpicc arranjo_no_scatter.c -o bin/arranjo_no_scatter
	mpicc arranjo_scatter.c -o bin/arranjo_scatter

clean:
	rm -r bin
	rm *.o