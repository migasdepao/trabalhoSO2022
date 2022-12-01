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

    struct pixel pix; //variavel de teste para brincar no while

    //metodo para obter tanto as coordenadas como os paths dos ficheiros binarios
    token[0] = strtok(buf, "  \n");
    for(int i=1; i<sizeof(token)/sizeof(token[0]); i++){
        token[i] = strtok(NULL, "  \n");
    }

    for(int i=0; i<sizeof(token)/sizeof(token[0]); i++){
        printf("%d: %s\n", i, token[i]);
    }

    int pid1, pid2;

    pid1 = fork();

    if(pid1 == 0) {
        //processo filho 1

        Coordenada teste;  //variavel de teste do tipo coordenada para poder ter os valores originais
        teste.latitude = atoi(token[0]); //copio a latitude original para a variavel de teste
        teste.longitude = atoi(token[1]);   //copio a longitude original para a variavel de teste

        FILE *fp0;

        fp0 = fopen(token[2], "rb"); //para já só abri este
        
        int v = 0; //variavel para saber em que pixel é que vou
        int k = 0; //variavel para incrementar em cada alarme acionado
            while((fread(&pix, sizeof(pix), 1, fp0)) != 0 & k < n){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if((pix.r + pix.g + pix.b) > 100 & pix.ir > 200){
                    alarmes[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmes[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados
                    k++;
                }
            v++;
            }
            
            printf("filho1 prestes a sair");
            
            _exit(1);   

    }   else{
        pid2 = fork();

        if(pid2 == 0){
        //processo filho 2

        Coordenada teste;  //variavel de teste do tipo coordenada para poder ter os valores originais
        teste.latitude = atoi(token[3]); //copio a latitude original para a variavel de teste
        teste.longitude = atoi(token[4]);   //copio a longitude original para a variavel de teste    

        FILE *fp1;

        fp1 = fopen(token[5], "rb");
        
        int v = 0; //variavel para saber em que pixel é que vou
        int k = 0; //variavel para incrementar em cada alarme acionado
            while((fread(&pix, sizeof(pix), 1, fp1)) != 0 & k < n){ //enquanto houver pixeis para ler e k<n em que k é a quantidade de alarmes e n é o maximo de alarmes
                printf("%d %d %d %d \n", pix.r, pix.g, pix.b, pix.ir);
                if((pix.r + pix.g + pix.b) > 100 & pix.ir > 200){
                    alarmes[k].latitude = teste.latitude;               //adiciono a alarmes a latitude original
                    alarmes[k].longitude = teste.longitude + v;         //adiciono a alarmes a longitude original mais a quantidade de pixeis procurados
                    k++;
                }
            v++;
            }

            printf("filho2 prestes a sair");
            _exit(1);
        }
        else{
            //processo pai
            while(wait(NULL) != -1);
            printf("latitude: %d\n logitude: %d\n", alarmes[0].latitude, alarmes[0].longitude); //print só de teste
            close(fd);

            return 0;


        }
    } 

}

int main(int argc, char argv[]){
    
    Coordenada* alarmes = malloc(sizeof(Coordenada));

    int n = pesquisaLote("./exemplo.txt", alarmes, 5);
    printf("Encontrei %d alarmes!", n);

    return 0;
}