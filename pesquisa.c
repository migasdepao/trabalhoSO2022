#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#include "satelite.c"

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
    
    int fd[2]; // pipe file descriptors


    free(alarmes);
    processoemail(n,alarmes);
    return 0;
    }

int processoemail(int n,Coordenada* alarmes) {
  int aux[2];
  int fd[2]; // pipe file descriptors
  char output[1024];

  // create the pipe
  if (pipe(fd) == -1) {
    perror("criaçao do pipe deu erro!");
    return 1;
  }
  
  
    dup2(fd[0], STDIN_FILENO); // redirect stdin
    close(fd[1]); // close the write end of the pipe
    execl("./mail.sh", "mail.sh", "bombeiros@protecao-civil.pt", (char*) NULL);

    
    close(fd[0]); // close the read end of the pipe
    for(int i = 0;  i < n ; i++){  // n - numero de alarmes
      aux[0]=alarmes[i].latitude;
      aux[1]=alarmes[i].longitude;
      write(fd[1], aux, sizeof(int)*2);
  }
  printf("%s",output);
  return 0;
}





