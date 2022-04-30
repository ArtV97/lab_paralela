# Laboratório de Programação Paralela
Disciplina do curso de Ciência da Computação da UFF (Universidade Federal Fluminense)

## Compilando e Executando
1) mpicc my_program.c -o program_executable
2) mpiexec -n \<number of process> program_executable

## DataTypes MPI
- MPI_CHAR
- MPI_INT
- MPI_FLOAT
- MPI_BYTE
- MPI_LONG
- MPI_UNSIGNED_CHAR, etc.

## Funções do MPI

### MPI_Send
``` C
int MPI_Send(void* message, int count, MPI_Datatype datatype,
int dest, int tag, MPI_Comm comm)
```
1) message: endereço do buffer de envio
2) count: número de elementos no buffer
3) datatype: tipo de dados do buffer
4) dest: o rank do processo destino
5) tag: tipo de mensagem a enviar ( * = MPI_ANY_TAG)
6) comm: Communicator

### MPI_Recv
``` C
int MPI_Recv(void* message, int count, MPI_Datatype datatype,
int source, int tag, MPI_Comm comm, MPI_Status* status)
```
1) message: endereço do buffer de recebimento
2) count: número máximo de elementos a serem recebidos
3) datatype: tipo de dados a serem recebidos
4) source: o rank do processo de origem da mensagem (* = MPI_ANY_SOURCE)
5) tag: tipo de mensagem a receber ( * = MPI_ANY_TAG)
6) comm: Communicator (como já visto)
7) status: uma estrutura que contém as informações:
``` C
    struct MPI_Struct {
        int MPI_SOURCE;
        int MPI_TAG;
        int MPI_ERROR;
    };
```

### MPI_Bcast
``` C
int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
```
1) buffer: endereço do buffer de envio/recebimento
2) count: número de elementos no buffer
3) datatype: tipo de dados do buffer (como já visto)
4) root: o rank do processo que fará o broadcast
5) comm: Communicator (como já visto)

### MPI_Reduce
``` C
int MPI_Reduce(const void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
```
1) sendbuf: endereço do buffer de envio
2) recvbuf: endereço do buffer de recebimento
3) count: número de elementos no buffer
4) datatype: tipo de dados do buffer (como já visto)
5) op: a operação de redução
6) root: o rank do processo que terá o resultado da operação de redução
7) comm: Communicator (como já visto)

### MPI_Allreduce
``` C
int MPI_Allreduce(const void *sendbuf, void *recvbuf,int count, MPI_Datatype datatype, MPI_Op op,MPI_Comm comm)
```
1) sendbuf: endereço do buffer de envio
2) recvbuf: endereço do buffer de recebimento
3) count: número de elementos no buffer
4) datatype: tipo de dados do buffer (como já visto)
5) op: a operação de redução
6) comm: Communicator (como já visto)