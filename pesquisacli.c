#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <syscall.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#define FIFO_FILE "server_fifo"
#define LOG "log"


int main(int argc, char *argv[]) {


// Crio o pipe para a passagem de dados do cliente para o servidor
mkfifo(FIFO_FILE, 0666);
// Crio o pipe para o envio da resposta do servidor para o cliente
mkfifo(LOG, 0666);

// Abri o FIFO file para poder escrever
int fd = open(FIFO_FILE, O_WRONLY);
// Abri o FIFO de log para poder ler a resposta do servidor
int fd1 = open(LOG, O_RDWR);


char paths[2][30];  //array de chars com duas posições -> [0] - guarda o path do ficheiro / [1] - guarda o valor maximo dos alarmes, neste caso passados ao dar exec do cli
strcpy(paths[0], argv[1]);
strcpy(paths[1], argv[2]);

char buffer[1024];  //array auxiliar para poder receber a resposta do servidor

write(fd, paths, sizeof(paths));      //o cliente manda o path e o limite para o servidor

int n = read(fd1, buffer, sizeof(buffer));

// se houve uma resposta dou print
if (n > 0) {
  printf("Received message: %s \n", buffer);
}

//Fecha os pipes
close(fd);
close(fd1);

return 0;
}