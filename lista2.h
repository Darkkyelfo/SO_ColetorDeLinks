#include<stdio.h>
#include <stdlib.h>
#include<string.h>

struct listaPalavras{
    int index;
    int quantidade;
    char *palavra;
    struct listaPalavras *prox;
};typedef struct listaPalavras ListaPalavras;


ListaPalavras* iniListaPalavras(){
    return NULL;
}

ListaPalavras* inserir(ListaPalavras* cabeca, char palavra[]){
    if(cabeca == NULL){
        ListaPalavras *cabeca = (ListaPalavras*)malloc( sizeof(ListaPalavras));
        cabeca->index=0;
        cabeca->palavra=(char*)malloc( sizeof(char)*strlen(palavra));
        strcpy(cabeca->palavra,palavra);
        cabeca->prox=NULL;
        cabeca->quantidade=1;
        return cabeca;
    }
    else{
        ListaPalavras *p,*n;
        for(p=cabeca;p!=NULL;p=p->prox){
            if(strcmp(p->palavra,palavra)==0){//Caso a palavra já esteja na lista, incremente a quantidade e retorne
                p->quantidade++;
                return cabeca;
            }
            n=p;
        }
        novo->index=n->index+1;
        novo->palavra=(char*)malloc(sizeof(char)*strlen(palavra)+1);
        novo->quantidade=1;
        strcpy(novo->palavra,palavra);
        novo->prox=NULL;
        n->prox=novo;
        free(n);


        return cabeca;
    }
}

char * recuperarPalavra(ListaPalavras *cabeca,int index){
    ListaPalavras *p;
    for(p=cabeca;p!=NULL;p=p->prox){
        if(p->index==index){
            return p->palavra;
        }
    }
    return "indice não existe na lista";
}



