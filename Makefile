# Este ficheiro pode ser modificado para acrescentar dependências adicionais 
# (outros ficheiros .h ou .c)
#
# Objetivo #1:
#     * A função pesquisaLote() deverá ser implementada num ficheiro
# 	satelite.c
# 
# Objetivo #2:
#     * Deverá ser implementado num ficheiro pesquisa.c
#
# Objetivo #3:
#     * Deverá ser implmentado nos ficheiros pesquisasrc.c e pesquisacli.c
#     * A sugestão de valorização deverá ser implementada em pesquisastop.c 
#
# Para gerar todos os objetivos pode executar na linha de comando:
#     make
#
# Para gerar um objetivo particular, por exemplo, o executável pesquisa:
#     make pesquisa

CFLAGS=-Wall -g

all: pesquisa pesquisacli pesquisasrv

# Comente a linha anterior e descomente a linha seguimte caso responda
# à sugestão de valorização do Objetivo #3.
#all: pesquisa pesquisacli pesquisasrv pesquisastop

pesquisa: pesquisa.c satelite.h satelite.c

pesquisacli: pesquisa.c satelite.h satelite.c

pesquisasrv: pesquisasrv.c satelite.h satelite.c

pesquisastop: pesquisastop.c
