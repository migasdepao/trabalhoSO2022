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

// Create the FIFO file
mkfifo(FIFO_FILE, 0666);

// Create the LOG file
mkfifo(LOG, 0666);

// Abri o FIFO file para poder ler e escrever
int fd = open(FIFO_FILE, O_RDWR);

// Abri o FIFO de log para poder escrever do cliente
int fd1 = open(LOG, O_RDWR);

char path[30];
strcpy(path, "./exemplo.txt");

printf("argv[1]: %s", path);

// fica continuamente à espera de respostas do servidor

printf("teste dento do whule");
char buffer[1024];
//o cliente precisa de escrever o path e o limite

write(fd, path, strlen(path));

int n = read(fd1, buffer, sizeof(buffer));

// se houve uma resposta dou print
if (n > 0) {
  printf("Received message: %s \n", buffer);
}



// Close the FIFO file and delete it
close(fd);
close(fd1);
unlink(FIFO_FILE);
unlink(LOG);

return 0;
}