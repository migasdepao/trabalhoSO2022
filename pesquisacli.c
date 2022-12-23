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
if (mkfifo(FIFO_FILE, 0666) < 0) {
perror("Erro ao criar o fifo file");
exit(1);
}

// Create the LOG file
if (mkfifo(LOG, 0666) < 0) {
perror("Erro ao criar o fifo file");
exit(1);
}

// Abri o FIFO file para poder ler e escrever
int fd = open(FIFO_FILE, O_RDWR);
if (fd < 0) {
perror("Erro ao abrir o fifo file");
exit(1);
}

// Abri o FIFO de log para poder escrever do cliente
int fd1 = open(LOG, O_RDWR);
if (fd1 < 0) {
perror("Erro ao abrir o log file");
exit(1);
}


printf("arv[0]: %s", argv[0]);
printf("arv[1]: %s", argv[1]);
printf("arv[2]: %s", argv[2]);
// fica continuamente à espera de respostas do servidor
while (1) {
printf("teste dento do whule");
char buffer[1024];
//o cliente precisa de escrever o path e o limite

//write(fd, argv, sizeof(argv));

int n = read(fd1, buffer, sizeof(buffer));

// se houve uma resposta dou print
if (n > 0) {
  printf("Received message: %s \n", buffer);
}

}

// Close the FIFO file and delete it
close(fd);
close(fd1);
unlink(FIFO_FILE);
unlink(LOG);

return 0;
}