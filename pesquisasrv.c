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
#include "pesquisa.c"
#define FIFO_FILE "server_fifo"
#define LOG "log"

typedef struct FIFO_MSG {char *path;
                 int limite;}fifo_msg;


int main(int argc, char *argv[]) {

//declaro um struct auxiliar
fifo_msg fm;
char* path;

// Create the FIFO file
mkfifo(FIFO_FILE, 0666);

// Create the LOG file
mkfifo(LOG, 0666);

// Abri o FIFO file para poder ler do servidor
int fd = open(FIFO_FILE, O_RDONLY);
if (fd < 0) {
perror("Erro ao abrir o fifo file");
exit(1);
}

// Abri o FIFO de log para poder escrever do server
int fd1 = open(LOG, O_WRONLY);
if (fd1 < 0) {
perror("Erro ao abrir o fifo file");
exit(1);
}

int limite = 5;

// fica continuamente à espera de respostas do cliente



int n = read(fd, path, strlen(path));
printf("li: %s\n", path);
// se houve uma resposta vai fazer algo
if (n > 0) {
  //printf("Received message: %s %d\n", fm.path, fm.limite);
    
  // Process the message...

  //obter o numero de linhas do exemplo.txt
    int fd1 = open(path, O_RDONLY);
    char c;
    int lines = 1;
    while (read(fd1, &c, 1) == 1) {
    // Increment the line count whenever a newline character is read
    if (c == '\n') {
      lines++;
        }
    }
    
    int maxAlarmes = limite; //é o numero máximo de alarmes que quero encontrar por .dat

    //aloco o tamanho da struct * o numero de ficheiros que vou procurar * o numero maximo de alarmes por ficheiro
    Coordenada* alarmes = malloc(sizeof(Coordenada) * lines * maxAlarmes); 

    int n = pesquisaLote(path, alarmes, maxAlarmes);
    printf("Encontrei %d alarmes!\n", n);
    for(int i = 0; i<n; i++){
        printf("latitude: %d longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
    }

    //execlp("sort", "-n", alarmes, n, sizeof(int), NULL);

    /*printf("depois do exec!\n");
    for(int i = 0; i<n; i++){
        printf("latitude: %d longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
    }*/
  
    
    ProcessoEmail(n,alarmes);
    free(alarmes);
    

  // Send a response to the client
  write(fd1, "Pesquisa Completa", 17);


}

// Close the FIFO file and delete it
close(fd);
close(fd1);
unlink(FIFO_FILE);

return 0;
}