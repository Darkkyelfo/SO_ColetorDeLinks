#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int tamanho=0;
struct lista{
    int index;
    int tamanho;
    char *url;
    struct lista *prox;
};typedef struct lista Lista;


Lista* inicializar(){
    return NULL;
}

Lista* inserirFinal(Lista* cabeca, char url[])
{
    if(cabeca == NULL){
        Lista *cabeca = (Lista*)malloc( sizeof(Lista));
        cabeca->index=0;
        cabeca->url=(char*)malloc(sizeof(char)*strlen(url)+10);
        strcpy(cabeca->url,url);
        cabeca->prox=NULL;
        cabeca->tamanho=1;
        return cabeca;
    }
    else
    {
        if(jaEstaNaLista(cabeca,url)==0){
            Lista *p=cabeca;
            while (p->prox!=NULL){
                p->tamanho=p->tamanho+1;
                p=p->prox;
            }
            p->tamanho=p->tamanho+1;
            Lista *novo = (Lista*)malloc(sizeof(Lista));
            novo->index=p->index+1;
            novo->url=(char*)malloc( sizeof(char)*strlen(url)+10);
            novo->tamanho=p->tamanho;
            strcpy(novo->url,url);
            novo->prox=NULL;
            p->prox=novo;
            p=NULL;
            free(p);
        }

        return cabeca;
    }
}

char * recuperarUrl(Lista *cabeca,int index){
    Lista *p;
    for(p=cabeca;p!=NULL;p=p->prox){
        if(p->index==index){
            return p->url;
        }
    }
    return "indice não existe na lista";
}

int jaEstaNaLista(Lista *cabeca, char url[]){
    Lista *p;
    for(p=cabeca;p!=NULL;p=p->prox){
        if(strcmp(p->url,url)==0){
            return 1;
        }
    }

    return 0;
}


