#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {

	int array[1000];
	int statusGlobal=0;
	int pdf[2];
	

	pipe(pdf);

	for(int i = 0; i<1000; i++){
		array[i]=0;
	}

	array[300] = 1;
	array[700] = 1;

	//o processo pai vai criar 4 filhos
	for(int i=0; i<3;i++){
		int e;
		int f = fork();
		if(f == 0){
			close(pdf[0]);
			//cada filho nasce aqui vai procurar os numeros e morre no _exit
			// fazer a minha tarefa = i
			for(int n = i*250; n<(250*(i+1)); n++){
				if(array[n]==1){
					//print encontrei - mas ia aparecer 4 vezes ou não
					//queremos comunicar ao pai o resultado
					write(pdf[1], &n, sizeof(int));
					printf("Encontrei e escrevi");
					e=1;
				}
			}
			close(pdf[1]);
					if(e==1){_exit(1);
					} else {_exit(0);}
		}
	}
	close(pdf[1]);
	int resultado[1000];

	
			int n=read(pdf[0], resultado, sizeof(int));
			int x=0;
			while(n>0){
				x++;
				n=read(pdf[0], resultado+x, sizeof(int));
				
			}
			close(pdf[0]);
			printf("Encontrei em [ ");
			for(int i=0; i<x; i++){
				printf(" %d ,", resultado[i]);
			}
					printf("]");
			

	
	//o processo pai fica à espera 4 vezes até que morram todos os processos filhos
	for(int i=0; i<3;i++){
		int status;
		wait(&status);

		if(WIFEXITED(status)){
				if(WEXITSTATUS(status)==1){
					statusGlobal = 1;
				}
		}
	}
	
	/*if(statusGlobal == 1){
		int n=read(pdf[0], resultado, sizeof(int));
		int x=0;
		while(n>0){
			n=read(pdf[0], resultado+x, sizeof(int));
			x++;
		}
		close(pdf[0]);
		printf("Encontrei em [ ");
		for(int i=0; i<x; i++){
			printf(" %d ,", resultado[i]);
		}
				printf("]");
		} else{
		printf("Não encontrei!");
	}*/
		close(pdf[0]);	
	
	}
		
	
	

