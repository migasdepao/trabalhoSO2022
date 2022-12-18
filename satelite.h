/* IMPORTANTE: Este ficheiro não pode ser alterado! */

#ifndef __SATELITE_H
#define __SATELITE_H

/* Formato dos ficheiros binários */
typedef struct pixel { unsigned int r, g, b, ir; }Pixel;

/* Formato da resposta da função pesquisaLote */
typedef struct coordenada { int latitude, longitude; }Coordenada;

/* Função para pesquisar situações de alarme num lote de dados */
int pesquisaLote(char* ficheiro, struct coordenada* alarmes, int n);

#endif