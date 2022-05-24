# Laboratório de Programação Paralela
Disciplina do curso de Ciência da Computação da UFF (Universidade Federal Fluminense)

## MPI
### Compilando e Executando
1) mpicc my_program.c -o program_executable
2) mpiexec -n <number of process> program_executable

## openMP
### Compilando e Executando
1) gcc my_program.c -o program_executable -fopenmp
2) ./program_executable
