O que cada thread está fazendo? 
Cada thread realiza parte do cálculo necessário do valor de PI.




Para que está sendo utilizado o pragma critical?
A variável pi é compartilhada, portanto, quando um processo for realizar uma escrita nessa variável, os outros processos que desejam ler ou escrever nela devem entrar na fila. Isso deve ser feito para evitar incosistência do dado.