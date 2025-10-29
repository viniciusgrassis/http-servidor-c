#include "http_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char* obterNomeArquivoDaUrl(const char* url){
    const char* nome_arquivo = strrchr(url, '/');
    if(nome_arquivo && strlen(nome_arquivo) > 1){
        return nome_arquivo + 1;
    }
    return "index.html";
}

int analisarUrl(const char* url, char* host, char* caminho, int* porta){
    if(strncmp(url, "http://", 7) != 0){
        fprintf(stderr, "Erro: URL deve começar com 'http://'\n");
        return -1;
    }
    char* url_sem_http = (char*)url + 7;

    char* inicio_caminho = strchr(url_sem_http, '/');
    char* inicio_porta = strchr(url_sem_http, ':');

    char* fim_host;

    if(inicio_caminho == NULL){
        strcpy(caminho, "/");
        fim_host = url_sem_http + strlen(url_sem_http);
    }else{
        strcpy(caminho, inicio_caminho);
        fim_host = inicio_caminho;
    }

    if(inicio_porta != NULL && inicio_porta < fim_host){
        int tamanho_host = inicio_porta - url_sem_http;
        strncpy(host, url_sem_http, tamanho_host);
        host[tamanho_host] = '\0';
        
        *porta = atoi(inicio_porta + 1);
        if(*porta == 0){
            fprintf(stderr, "Erro: Porta inválida '%s'\n", inicio_porta + 1);
            return -1;
        }
    }else{
        int tamanho_host = fim_host - url_sem_http;
        strncpy(host, url_sem_http, tamanho_host);
        host[tamanho_host] = '\0';
    }
    return 0;
}

const char* obterTipoConteudo(const char* caminho){
    const char* extensao = strrchr(caminho, '.');
    if(!extensao) return "application/octet-stream";

    if(strcmp(extensao, ".html") == 0) return "text/html";
    if(strcmp(extensao, ".css") == 0) return "text/css";
    if(strcmp(extensao, ".js") == 0) return "application/javascript";
    if(strcmp(extensao, ".txt") == 0) return "text/plain";
    if(strcmp(extensao, ".jpg") == 0 || strcmp(extensao, ".jpeg") == 0) return "image/jpeg";
    if(strcmp(extensao, ".png") == 0) return "image/png";
    if(strcmp(extensao, ".gif") == 0) return "image/gif";

    return "application/octet-stream";
}