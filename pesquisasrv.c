#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syscall.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "satelite.h"
#define FIFO_FILE "server_fifo"
#define LOG "log"

int main(int argc, char *argv[]) {

// Crio o pipe para a passagem de dados do cliente para o servidor
mkfifo(FIFO_FILE, 0666);
// Crio o pipe para o envio da resposta do servidor para o cliente
mkfifo(LOG, 0666);

// Abri o FIFO file para poder ler do cliente
int fd = open(FIFO_FILE, O_RDONLY);
// Abri o FIFO de log para poder escrever a resposta para o cliente
int fd1 = open(LOG, O_WRONLY);

char paths[2][30];  //array de chars com duas posições -> [0] - guarda o path do ficheiro / [1] - guarda o valor maximo dos alarmes

// fica continuamente à espera de respostas do cliente

while(1){

int n = read(fd, paths, sizeof(paths));         //leio o que é passado do cliente

// se houve uma resposta vai fazer algo
if (n > 0) {
    printf("li: %s %s\n", paths[0], paths[1]);
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0){
      // processo filho
    execlp("./pesquisa", "pesquisa", paths[0], paths[1], (char *)NULL);               //pesquisaLote(path, alarmes, maxAlarmes);

    }else{
      //processo pai

      // Envia a resposta para o cliente
      write(fd1, "Pesquisa Completa", 17);

    }
  
}

}

// Fecha os pipes
close(fd);
close(fd1);

return 0;
}