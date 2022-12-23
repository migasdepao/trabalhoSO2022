#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <syscall.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#define FIFO_FILE "server_fifo"

typedef struct FIFO_MSG {char path[100];
                 int limite;}fifo_msg;

int main(int argc, char *argv[]) {

//declaro um struct auxiliar
fifo_msg fm;

/* Create the FIFO file
if (mkfifo(FIFO_FILE, 0666) < 0) {
perror("Erro ao criar o fifo file");
exit(1);
}*/

// Abri o FIFO file para poder ler e escrever
int fd = open(FIFO_FILE, O_RDWR);
if (fd < 0) {
perror("Erro ao abrir o fifo file");
exit(1);
}

// fica continuamente à espera de respostas do servidor
while (1) {

char buffer[1024];
//o cliente precisa de escrever o path e o limite

write(fd, &fm, sizeof(fifo_msg));

int n = read(fd, buffer, sizeof(buffer));

// se houve uma resposta dou print
if (n > 0) {
  printf("Received message: %s \n", buffer);
}

}

// Close the FIFO file and delete it
close(fd);
unlink(FIFO_FILE);

return 0;
}