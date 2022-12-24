#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"
#define MAX_PROCESSES 4

//struct pixel { unsigned int r, g, b, ir; };
//struct coordenada { int latitude, longitude; };


int pesquisaLote(char* ficheiro, struct coordenada* alarmes, int n){

    //obter o numero de linhas do exemplo.txt
    int fd1 = open(ficheiro, O_RDONLY);
    char c;
    int lines = 1;
    while (read(fd1, &c, 1) == 1) {
    // Increment the line count whenever a newline character is read
    if (c == '\n') {
      lines++;
        }
    }
    printf("numero de linhas\n: %d", lines);
    
    int fd = open(ficheiro, O_RDONLY);
    
    char buf[(35*lines)];   //array para obter a string do exemplo.txt (27 para conseguir ler uma linha inteira * nr linhas)
    read(fd, &buf, (35*lines));
    char* token[(3*lines)]; //tamanho do token será 3 (lat + long + path) * numero de linhas
    int alarmesTotais=0;

    struct pixel pix; //variavel de teste para brincar no while
    
    //metodo para obter tanto as coordenadas como os paths dos ficheiros binarios
    token[0] = strtok(buf, "  \n");
    for(int i=1; i<sizeof(token)/sizeof(token[0]); i++){
        token[i] = strtok(NULL, "  \n");
    }

    //print dos tokens
    for(int i=0; i<sizeof(token)/sizeof(token[0]); i++){
        printf("%d: %s\n", i, token[i]);
    }

    //lines vai ser a quantidade total de processos que vamos criar

    int pfd[lines][2];

    //crio os pipes e verifico logo se ocorreu algum erro
    for(int i=0; i<lines; i++){
        if(pipe(pfd[i]) < 0 ){
            perror("Erro ao criar o pipe");
            return 1;
        }
    }



    //criamos um processo para cada .dat que vamos procurar
    pid_t filhos_pids[lines]; //crio a quantidade de pids conforme a quantidade de linhas, neste caso será igual ao valor total de ficheiros em que vamos procurar
    pid_t pid;
    int globalInd = 0; //index global para conseguir guardar os valores dos alarmes
    int processos_concurrentes=0;
        
    for(int p=0;p<lines;p++)
    {
            while(processos_concurrentes == MAX_PROCESSES)
                {
                int status;
                pid_t filho_pid=waitpid(-1,&status,WNOHANG);
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
            teste.latitude = atoi(token[(0+(p*3))]); //copio a latitude original para a variavel de teste
            teste.longitude = atoi(token[(1+(p*3))]);   //copio a longitude original para a variavel de teste

            int fp0 = open(token[(2+(p*3))], O_RDONLY);

            int v = 0; //variavel para saber em que pixel é que vou
            int k = 0; //variavel para incrementar em cada alarme acionado
            int aux[2];
            Coordenada* alarmesAux = malloc(sizeof(struct coordenada));

            while( ( (read(fp0, &pix, sizeof(pix)) ) > 0 )  & (k < n)){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                //printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if(((pix.r + pix.g + pix.b) > 100) & (pix.ir > 200)){
                    alarmesAux[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmesAux[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados
                    
                    aux[0]=alarmesAux[k].latitude;
                    aux[1]=alarmesAux[k].longitude;
                    k++;
                    //printf("processo %d: aux[0] = %d\n processo %d: aux[1] = %d\n",p,aux[0],p, aux[1]);
                    write(pfd[p][1], aux, sizeof(int)*2 );
                }
            v++;  
            }
              
            close(pfd[p][1]);
            printf("Filho %d prestes a sair\n", p);

            _exit(k);
      
        }
        else{
            //processo pai
            
            processos_concurrentes++;
            filhos_pids[p]= pid;  
            
            close(pfd[p][1]);
               
            int x=0;
            int aux[2];
            
            //alarmes = realloc(alarmes, sizeof(struct coordenada) * n);

            int c = read(pfd[p][0], aux, sizeof(int)*2);

            while(c>0){
                //printf("dentro do while:%d \n", x);
                alarmes[globalInd].latitude = aux[0];
                alarmes[globalInd].longitude = aux[1];
                globalInd++;
                x++;
                c = read(pfd[p][0], aux, sizeof(int)*2);
            }
           
            for(int i=0; i<x; i++){

                //printf("FOR: latitude: %d\n FOR: longitude: %d\n", alarmes[i].latitude, alarmes[i].longitude);
            }
           

            //printf("latitude: %d\n logitude: %d\n", alarmes[0].latitude, alarmes[0].longitude); //print só de teste
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

    


       



    
        //espero todos os processos terminar em caso de duvida
    //     for(int p=0; p<lines; p++){
    //             int status;
    //         waitpid(pids[p], &status, 0);
    //         if(WIFEXITED(status))
    //             alarmesTotais += WEXITSTATUS(status);
              
    //         }
       

    //     return alarmesTotais;
        
    // }
    

/*
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
    
    free(alarmes);
    
    return 0;


}*/