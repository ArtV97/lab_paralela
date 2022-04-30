O problema a ser solucionado é, dado um elemento x e um conjunto de elementos do mesmo tipo, S, responder a pergunta: x ∊ S ?
Faça uma solução paralela em MPI em que, x e S são dados como entrada (pode considerar que cada elemento é um inteiro e S é um vetor de inteiros), sendo que:
VERSÃO 1: x é informado pelo usuário e um vetor S é gerado aleatoriamente e tem tamanho N.
VERSÃO 2: x e o vetor S de tamanho N são lidos de um arquivo.

Considere que o número de processadores é P.

1) Implemente uma versão distribuída utilizando a biblioteca MPI, com send e receives
2) Implemente uma outra versão com MPI_Scatter (dispersão)
3) Compare o tempo de execução e aponte as vantagens e desvantagens