#!/bin/bash


INPUTS=( "fin1.txt" "fin2.txt" "fin3.txt" "fin4.txt" )
N=( "1" "2" "4" )

# clean
make clean
# build
make

echo 
for filename in ${INPUTS[@]}
do
    ./bin/arranjo_seq -f ${filename}

    for n in ${N[@]}
    do
        echo "_________________________________________________________________________________"
        echo "$n threads/processes"
        ./bin/arranjo_openmp -num_threads ${n} -f ${filename}
        mpiexec -n ${n} bin/arranjo_no_scatter -f ${filename}
        mpiexec -n ${n} bin/arranjo_scatter -f ${filename}
    done

    echo "====================================================================================="
done
