#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include "satelite.h"

//struct pixel { unsigned int r, g, b, ir; };
//struct coordenada { int latitude, longitude; };


int pesquisaLote(char* ficheiro, struct coordenada* alarmes, int n){
    int fd = open(ficheiro, O_RDONLY);
    
    char buf[200]; //array para obter a string do exemplo.txt
    int x = read(fd, &buf, 60);
    char* token[6]; //coloquei 6 só para testar mas caso se aumentasse a quantidade de coordenadas a pesquisar também teriamos que aumentar ao tamanho do token
    int alarmesTotais=0;

    struct pixel pix; //variavel de teste para brincar no while

    //metodo para obter tanto as coordenadas como os paths dos ficheiros binarios
    token[0] = strtok(buf, "  \n");
    for(int i=1; i<sizeof(token)/sizeof(token[0]); i++){
        token[i] = strtok(NULL, "  \n");
    }

    for(int i=0; i<sizeof(token)/sizeof(token[0]); i++){
        printf("%d: %s\n", i, token[i]);
    }

    int pfd[2][2];
    int pid1, pid2;

    for(int i=0; i<2; i++){
        if(pipe(pfd[i]) < 0){
            return -1;
        }
    }

    pid1 = fork();

    if(pid1 == 0) {
        //processo filho 1

        close(pfd[0][0]);
        close(pfd[1][0]); //dou close no processo 1 o pipe que será usado pelo outro processo
        close(pfd[1][1]); //dou close no processo 1 o pipe que será usado pelo outro processo

        Coordenada teste;  //variavel de teste do tipo coordenada para poder ter os valores originais
        teste.latitude = atoi(token[0]); //copio a latitude original para a variavel de teste
        teste.longitude = atoi(token[1]);   //copio a longitude original para a variavel de teste


        int fp0 = open(token[2], O_RDONLY); //para já só abri este
        
        int v = 0; //variavel para saber em que pixel é que vou
        int k = 0; //variavel para incrementar em cada alarme acionado
        int aux[2];
            while((read(fp0, &pix, sizeof(pix))) > 0  & k < n){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if((pix.r + pix.g + pix.b) > 100 & pix.ir > 200){
                    alarmes[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmes[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados
                    
                    aux[0]=alarmes[k].latitude;
                    aux[1]=alarmes[k].longitude;
                    k++;
                    printf("processo 1: aux[0] = %d\n processo1: aux[1] = %d\n", aux[0], aux[1]);
                    write(pfd[0][1], aux, sizeof(int)*2 );
                }
            v++;
            }
            
            close(pfd[0][1]);
            printf("filho1 prestes a sair");
            
            _exit(k);   

    }   else{
        pid2 = fork();

        if(pid2 == 0){
        //processo filho 2

        close(pfd[0][0]);
        close(pfd[0][1]);
        close(pfd[1][0]);

        Coordenada teste;  //variavel de teste do tipo coordenada para poder ter os valores originais
        teste.latitude = atoi(token[3]); //copio a latitude original para a variavel de teste
        teste.longitude = atoi(token[4]);   //copio a longitude original para a variavel de teste    


        int fp1 = open(token[5], O_RDONLY);
        
        int v = 0; //variavel para saber em que pixel é que vou
        int k = 0; //variavel para incrementar em cada alarme acionado
        int aux[2];
            //read(fp1, &pix, sizeof(pix))
            while((read(fp1, &pix, sizeof(pix))) > 0 & k < n){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if((pix.r + pix.g + pix.b) > 100 & pix.ir > 200){
                    alarmes[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmes[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados

                    aux[0]=alarmes[k].latitude;
                    aux[1]=alarmes[k].longitude;

                    k++;
                    printf("processo 2: aux[0] = %d\n processo2: aux[1] = %d\n", aux[0], aux[1]);
                    write(pfd[1][1], aux, sizeof(int)*2 );
                }
            v++;
            }
            close(pfd[1][1]);

            printf("filho2 prestes a sair");
            _exit(k);
        }
        else{
            //processo pai

            close(pfd[0][1]);
            close(pfd[1][1]);    

            int x=0;
            int aux[2];
            
            Coordenada* alarmesAux = malloc(sizeof(struct coordenada) * n);

            int c = read(pfd[0][0], aux, sizeof(int)*2);

            while(c>0){
                printf("dentro do while:%d \n", x);
                alarmesAux[x].latitude = aux[0];
                alarmesAux[x].longitude = aux[1];
                x++;
                c = read(pfd[0][0], aux, sizeof(int)*2);
            }
           
            for(int i=0; i<x; i++){

                printf("FOR: latitude: %d\n FOR: longitude: %d\n", alarmesAux[i].latitude, alarmesAux[i].longitude);
            }

            for(int i=0; i<2; i++){
                int status;
            wait(&status);
            if(WIFEXITED(status))
                alarmesTotais += WEXITSTATUS(status);
              
            }

            //printf("latitude: %d\n logitude: %d\n", alarmes[0].latitude, alarmes[0].longitude); //print só de teste
            close(fd);

            return alarmesTotais;


        }
    } 

}

int main(int argc, char argv[]){
    
    Coordenada* alarmes = malloc(sizeof(Coordenada));

    int n = pesquisaLote("./exemplo.txt", alarmes, 5);
    printf("Encontrei %d alarmes!\n", n);
    printf("teste de escrita alarmes %d %d", alarmes[0].latitude, alarmes[0].longitude);

    return 0;
}