#include<unistd.h>
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<errno.h>
#include "lista.h"
#include<lista2.h>
#include <pthread.h>


#define maxLink 100
Lista *lURLsAchadas= NULL;
int indexThread=0;
pthread_t t[200];

//verifica se o link é um css
short verificarCSS(char url[]){
    char final[4]="";
    strcat(final,&url[strlen(url)-3]);

    if(strcmp(final,"css")==0 || strcmp(final,"ss/")==0){
        return 1;
    }
    return 0;
}

/*Essa função baixar o html de uma page
pra isso ela utiliza o comando wget do
linux*/
char* getHTML(char url[]){
    char link[1000]="wget -T 5 --tries=1 '";
    char *nomeArq=malloc(strlen(url) * sizeof(char)+1);;
    //troca a / por _
    int i;
    for(i=0;i<strlen(url);i++){
        nomeArq[i]=url[i];
        if(url[i]=='/'){
            nomeArq[i]='_';
        }
    }
    char comandoO[1000]=" -O '";//guarda o comando para renomear o arquivo.
    strcat(comandoO,nomeArq);
    //coloca aspas no nome do arquivo. Evita erros na hora de criar o arquivo html
    strcat(comandoO,"'");
    //concatena as strings para formar o comando wget. "wget url -O nomeArquivo"
    strcat(link,url);
    strcat(link,"'");
    strcat(link,comandoO);
    system(link);//executa o comando wget
    return nomeArq;
}

char* getTexto(char url[]){  
    char link[1000]="w3m '";
    strcat(link,url);
    strcat(link,"'");
    char *nomeArq=malloc(strlen(url) * sizeof(char));;

    //troca a / por _
    int i;
    for(i=0;i<strlen(url);i++){
        nomeArq[i]=url[i];
        if(url[i]=='/'){
            nomeArq[i]='_';
        }
    }
    char comandoO[1000]=" > ";//guarda o comando para renomear o arquivo.
    // strcat(nomeArq,".txt");
    strcat(comandoO,nomeArq);
    //concatena as strings para formar o comando w3m. "w3m 'url' > nomeArquivo"
    strcat(link,comandoO);
    system(link);//executa o comando wm3
    return nomeArq;
}

void *contarPalavras(char url[]){
    char *nomeArq=getTexto(url);//Guarda o nome do arquivo onde está o texto do html
    FILE *arqTXT;
    int letra;
    if((arqTXT = fopen(nomeArq, "r")) != NULL){
        char *palavra = (char*)malloc( sizeof(char)*70);
        ListaPalavras *lpalavras=iniListaPalavras();// lista que armazena as palavras encontradas e a quantidade de vezes que apareceram
        while((letra = fgetc(arqTXT)) != EOF){
            if(letra==':' || letra==' ' || letra=='.' || letra==',' || letra=='^' || letra == '\n' || letra=='-' || letra=='_' || letra=='/' || letra=='\\' || letra==')' || letra=='=' || letra=='?'){//indica que a palavra já foi encontrada
                if(strlen(palavra)>0){
                    lpalavras = inserir(lpalavras,palavra);//add a palavra a lista
                }
                strcpy(palavra,"");
            }
            else{
                if(strlen(palavra)<70 && letra!='"' && letra!='(' && letra!=39 && letra!='\033]' && letra>=0){
                    strcat(palavra,&letra);
                }
            }
        }
        free(palavra);
        char *nomeArqP = (char*)malloc( sizeof(char)*(strlen(nomeArq)+15));//arquivo que guarda a as palavras achadas e o número de vezes que apareceram
        strcpy(nomeArqP,nomeArq);
        strcat(nomeArqP," Palavras.txt");
        FILE *arqPalavras = fopen(nomeArqP,"w");
        ListaPalavras *p;
        fprintf(arqPalavras,"PALAVRA\t\t\tQUANTIDADE\n");
        for(p=lpalavras; p!=NULL;p=p->prox){
            fprintf(arqPalavras,p->palavra);
            fprintf(arqPalavras,"\t\t");
            char num[20];
            sprintf(num,"%d",p->quantidade);
            fprintf(arqPalavras,num);
            fprintf(arqPalavras,"\n\n");
        }
        fprintf(arqPalavras,"FIM DO ARQUIVO");
        fclose(arqTXT);

    }
    else{
        printf("\n---------- não foi possivel extrair as palavras dessa url------------------\n");
    }



}

/*função responsálve por coletar os links(http) existentes
na url passada como argumento. Primeiro ela realiza o download do html
da page, em seguida faz a busca pelos links*/
void *coletorDeLinks(void *url){
    int numLinks=0;//Guarda a quantidade de links achados
    //short temFilhos=0;
    char *nomeArq = (char*)malloc( sizeof(char)*strlen(url));
    strcpy(nomeArq,getHTML((char*)url));//Faz o download do arquivo html
    FILE *arqHTML;
    int letra;//variável que guarda o char que está sendo lido no momento.
    const char *encontrar="href=\"http://";//palavra que desejo encontrar. No caso todos que comecem com http
    short encontrouLink=0;//"flag" que sinaliza que um link foi encontrado
    short cont=0;
    if((arqHTML = fopen(nomeArq, "r")) != NULL)
    {

        //cria um arquivo que guarda os links encontrados na html
        char *nomeArqLinks=(char*)malloc(sizeof(char)*strlen(nomeArq)+25);
        strcpy(nomeArqLinks,nomeArq);
        strcat(nomeArqLinks,"links.txt");

        FILE *arquivoLink = fopen(nomeArqLinks,"w");
        char urls[800]="";//Guarda as urls encontradas;
        while((letra = fgetc(arqHTML)) != EOF){//Lê o arquivo letra por letra
            if(letra==encontrar[cont]){
                cont++;
                if(cont==strlen(encontrar)){//Se cont foi incrementado 5 vezes significa que a palavra "http" foi encontrada
                    encontrouLink=1;
                    cont=0;
                    numLinks++;
                    continue;
                }
            }
            else{
                cont=0;
            }
            if(encontrouLink==1){//monta a string do link encontrado
                if(letra=='<' || letra=='"' || letra=='>' || letra==39){//Serve para indicar quando o link acaba
                    fprintf(arquivoLink,urls);
                    fprintf(arquivoLink,"\n");

                    if((lURLsAchadas==NULL || lURLsAchadas->tamanho<maxLink) && verificarCSS(urls)==0){
                        lURLsAchadas=inserirFinal(lURLsAchadas,urls);//inseri os links achados em uma lista encadeada
                    }
                    encontrouLink=0;
                    strcpy(urls,"");
                }
                else{
                    if(letra!='\n' && letra>=0){
                        strcat(urls,&letra);
                    }
                }
            }
        }
        printf("\nO número de link achados foi:%d\n",numLinks);
        fclose(arqHTML);
        fclose(arquivoLink);

        char rm[] = "rm -r ";
        strcat(rm,nomeArq);
        system(rm);

        contarPalavras(url);

        char rm1[] = "rm -r ";
        strcat(rm1,nomeArq);
        system(rm1);
    }
    else
    {
        printf("\n---------- não foi possivel extrair os links dessa URL\n");
    }
}

void main(void){
    //coletorDeLinks("http://dc.wikia.com/wiki/Superman");
    coletorDeLinks("https://pt.wikipedia.org/wiki/Macaco");
    Lista *p=lURLsAchadas;
    int i=0;
    for(p=lURLsAchadas;p!=NULL;p=p->prox){
        i++;
        pthread_create(&t[p->index],NULL,coletorDeLinks,(void*)p->url);
        if(p->prox==NULL && p->tamanho<maxLink){//Caso o número de link desejados não seja encontrada espere a thread terminar de executar
            pthread_join(t[p->index],NULL);
        }
    }
    for(p=lURLsAchadas;p!=NULL;p=p->prox){
        pthread_join(t[p->index],NULL);
    }


    printf("\n------------------ ACABOU--------------------\n");
    exit(2);
}
