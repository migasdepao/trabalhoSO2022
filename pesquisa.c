#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"


int ProcessoEmail(int n,Coordenada* alarmes) {

  // crio o pipe para o processo filho comunicar
    int pfd[2];
    if (pipe(pfd) == -1) {
        perror("pipe");
        return -1;
    }

  // fork the process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

  if (pid == 0) {
        // processo filho
        close(pfd[1]); // close the write end of the pipe

        // coloco a parte de leitura do pipe para o stdin
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            return -1;
        }

        // executo o mail.sh
        execlp("./mail.sh", "./mail.sh", "bombeiros@protecao-civil.pt", (char*)NULL);
        perror("execlp");
        return -1;
    } else{

        // processo pai
        close(pfd[0]); // close the read end of the pipe

        // escreve os emails para o stdin
        if (write(pfd[1], alarmes, sizeof(struct coordenada)*n) == -1) {
            perror("write");
            return -1;
        }

        close(pfd[1]); // close the write end of the pipe

        wait(NULL);
    }


  return 0;
}


int main(int argc, char *argv[]){

    //caso queira ver o que recebeu da linha de comandos:
    // printf("argv[0] %s\n", argv[0]);
    // printf("argv[1] %s\n", argv[1]);
    // printf("argv[2] %d\n", atoi(argv[2]));

    
    char path[20];
    strcpy(path, argv[1]);
    int maxAlarmes = atoi(argv[2]); //é o numero máximo de alarmes que quero encontrar por .dat

    //obter o numero de linhas do exemplo.txt
    int fd1 = open(path, O_RDONLY);
    char c;
    int lines = 1;
    while (read(fd1, &c, 1) == 1) {
    // Incremento sempre que leio um \n, assumo que no inicio da leitura já tenho uma linha
    if (c == '\n') {
      lines++;
        }
    }

    //aloco o tamanho da struct * o numero de ficheiros que vou procurar * o numero maximo de alarmes por ficheiro
    Coordenada* alarmes = malloc(sizeof(Coordenada) * lines * maxAlarmes);

    int n = pesquisaLote(path, alarmes, maxAlarmes);
    printf("Encontrei %d alarmes!\n", n);
    for(int i = 0; i<n; i++){
        printf("latitude: %d longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
    }

    ProcessoEmail(n,alarmes);
    free(alarmes);

   return 0;
    }

