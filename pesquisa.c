#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#include "satelite.c"


int processoemail(int n,Coordenada* alarmes) {
  //int aux[2];
  

  // criar o pipe para o processo filho comunicar
    int pfd[2]; // pipe file descriptors
    if (pipe(pfd) == -1) {
        perror("pipe");
        return 1;
    }
  
  // fork the process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }
  
  if (pid == 0) {
        // child process
        close(pfd[1]); // close the write end of the pipe

        // set the read end of the pipe as the standard input
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            return 1;
        }

        // execute the mail.sh script with the email address as the argument
        execlp("./mail.sh", "./mail.sh", "bombeiros@protecao-civil.pt", (char*)NULL);
        perror("execlp");
        return 1;
    } else{

        // parent process
        close(pfd[0]); // close the read end of the pipe

        // escreve os emails para o stdin
        
        if (write(pfd[1], alarmes, sizeof(struct coordenada)*n) == -1) {
            perror("write");
            return 1;
        }
        

        close(pfd[1]); // close the write end of the pipe

        wait(NULL);
    }

  
  return 0;
}


int main(int argc, char argv[]){
    
    
    //obter o numero de linhas do exemplo.txt
    int fd1 = open("./exemplo.txt", O_RDONLY);
    char c;
    int lines = 1;
    while (read(fd1, &c, 1) == 1) {
    // Increment the line count whenever a newline character is read
    if (c == '\n') {
      lines++;
        }
    }
    
    int maxAlarmes = 5; //é o numero máximo de alarmes que quero encontrar por .dat

    //aloco o tamanho da struct * o numero de ficheiros que vou procurar * o numero maximo de alarmes por ficheiro
    Coordenada* alarmes = malloc(sizeof(Coordenada) * lines * maxAlarmes); 

    int n = pesquisaLote("./exemplo.txt", alarmes, maxAlarmes);
    printf("Encontrei %d alarmes!\n", n);
    for(int i = 0; i<n; i++){
        printf("latitude: %d longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
    }

    //execlp("sort", "-n", alarmes, n, sizeof(int), NULL);

    /*printf("depois do exec!\n");
    for(int i = 0; i<n; i++){
        printf("latitude: %d longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
    }*/
  
    
    int l = processoemail(n,alarmes);
    free(alarmes);
    
   return 0;
    }
