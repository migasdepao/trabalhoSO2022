#include <stdio.h>



int main(int argc, char const *argv[])

{

    int arr[30] = {1, 34, 1, 11, 

                   123, 100, 2, 10, 

                   23, 224, 253, 21,

                   40, 43, 40, 12,

                   42, 43, 44, 13,

                   100, 70, 21, 250,

                   12, 234, 2, 2};



    FILE *ficheiro;



    ficheiro = fopen("a05.dat", "wb");

    fwrite(arr, sizeof(int), 30, ficheiro);

    fclose(ficheiro);



    return 0;

}