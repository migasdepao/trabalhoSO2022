#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#define MAX_PROCESSES 4
#define MAX 100

int pesquisaLote(char* ficheiro, struct coordenada* alarmes, int n){

    //obter o numero de linhas do exemplo.txt
    int fd1 = open(ficheiro, O_RDONLY);
    char c;
    int lines = 1;
    while (read(fd1, &c, 1) == 1) {
    // Incremento sempre que leio um \n, assumo que no inicio da leitura já tenho uma linha
    if (c == '\n') {
      lines++;
        }
    }
    
    int fd = open(ficheiro, O_RDONLY);
    
    char buf[(MAX*lines)];   //array para obter a string do exemplo.txt (27 para conseguir ler uma linha inteira * nr linhas)
    int r = read(fd, &buf, (MAX*lines));
    buf[r] = '\0';
    char* token[(3*lines)]; //tamanho do token será 3 (lat + long + path) * numero de linhas
    int alarmesTotais=0;

    struct pixel pix; //variavel auxiliar para guardar os pixeis nas leituras
    
    //metodo para obter tanto as coordenadas como os paths dos ficheiros binarios
    token[0] = strtok(buf, ",  \n");
    for(int i=1; i<sizeof(token)/sizeof(token[0]); i++){
        token[i] = strtok(NULL, ",  \n");
    }

    //se quiser dar print dos tokens:
    // for(int i=0; i<sizeof(token)/sizeof(token[0]); i++){
    //     printf("%d: %s\n", i, token[i]);
    // }

    int pfd[lines][2];

    //crio os pipes e verifico logo se ocorreu algum erro
    for(int i=0; i<lines; i++){
        if(pipe(pfd[i]) < 0 ){
            perror("Erro ao criar o pipe");
            return 1;
        }
    }

    //criamos um processo para cada .dat que vamos procurar
    pid_t filhos_pids[lines];   //crio a quantidade de pids conforme a quantidade de linhas, neste caso será igual ao valor total de ficheiros em que vamos procurar
    pid_t pid;                  //id de um processo atual que será adicionado ao filhos_pid
    int globalInd = 0;          //index global para conseguir guardar os valores dos alarmes
    int processos_concurrentes=0;
        
    for(int p=0;p<lines;p++)
    {
            while(processos_concurrentes == MAX_PROCESSES)
                {
                int status;
                pid_t filho_pid=waitpid(-1,&status,WNOHANG);    //fico à espera que qualquer processo acabe
                if(filho_pid>0)
                {
                 printf("Status=%d\t  Pid=%d\n",WEXITSTATUS(status),filho_pid);
                 alarmesTotais+=WEXITSTATUS(status);
                 processos_concurrentes--;
                }
          
            }
        pid = fork();

        if(pid==0)
        {
          //processo filho
            close(pfd[p][0]);

            Coordenada teste;  //variavel de teste do tipo coordenada para poder ter os valores originais
            teste.latitude = atoi(token[(0+(p*3))]);    //copio a latitude original para a variavel de teste
            teste.longitude = atoi(token[(1+(p*3))]);   //copio a longitude original para a variavel de teste

            int fp0 = open(token[(2+(p*3))], O_RDONLY);

            int v = 0;  //variavel para saber em que pixel é que vou
            int k = 0;  //variavel para incrementar em cada alarme acionado
            int aux[2]; //array de inteiros para nos auxiliar ao guardar os valores das coordenadas
            Coordenada* alarmesAux = malloc(sizeof(struct coordenada));

            while( ( (read(fp0, &pix, sizeof(pix)) ) > 0 )  && (k < n)){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                //printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if(((pix.r + pix.g + pix.b) > 100) & (pix.ir > 200)){
                    alarmesAux[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmesAux[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados
                    
                    aux[0]=alarmesAux[k].latitude;
                    aux[1]=alarmesAux[k].longitude;
                    k++;
                    write(pfd[p][1], aux, sizeof(int)*2 );         //mando para o processo pai as coordenadas
                }
            v++;  
            }
              
            close(pfd[p][1]);
            //printf("Filho %d prestes a sair\n", p); //caso queira saber em que filho estou

            _exit(k);
      
        }
        else{
            //processo pai
            
            processos_concurrentes++;
            filhos_pids[p]= pid;  
            
            close(pfd[p][1]);
               
            int x=0;
            int aux[2];
        

            int c = read(pfd[p][0], aux, sizeof(int)*2);    //leio do filho os valores das coordenadas

            //loop para guardar no alarmes todas as coordenadas dos incêndios encontrados
            while(c>0){ 
                alarmes[globalInd].latitude = aux[0];   
                alarmes[globalInd].longitude = aux[1];
                globalInd++;
                x++;
                c = read(pfd[p][0], aux, sizeof(int)*2);
            }

            close(fd);
            }

    }

       //espero todos os processos terminar em caso de duvida
        for(int t=0; t<lines; t++){
                int status;
            pid_t pid=waitpid(-1, &status, 0);
            if(pid>0){
                printf("Status=%d\t  PID=%d\n",WEXITSTATUS(status),filhos_pids[t]);
                alarmesTotais+=WEXITSTATUS(status);

            } 
        }
      
        return alarmesTotais;

    }