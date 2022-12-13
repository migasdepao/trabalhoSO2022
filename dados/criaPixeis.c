#include <stdio.h>



int main(int argc, char const *argv[])

{

    int arr[30] = {1, 34, 1, 313, 

                   123, 2, 2, 32, 

                   23, 224, 253, 21,

                   23, 43, 32, 250,

                   42, 43, 44, 445,

                   1, 1, 21, 41,

                   12, 234, 2, 2};



    FILE *ficheiro;



    ficheiro = fopen("a03.dat", "wb");

    fwrite(arr, sizeof(int), 30, ficheiro);

    fclose(ficheiro);



    return 0;

}