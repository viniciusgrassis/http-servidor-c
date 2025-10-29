#include "http_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <arpa/inet.h>

#define TAMANHO_BUFFER 4096

int executarHttpGet(const char* host, int porta, const char* caminho, const char* nome_arquivo_saida){
    struct hostent* servidor = gethostbyname(host);
    if(servidor == NULL){
        fprintf(stderr, "Erro: Não foi possível encontrar o host '%s'\n", host);
        return -1;
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0){
        perror("Erro ao abrir o socket");
        return -1;
    }

    struct sockaddr_in endereco_servidor;
    endereco_servidor.sin_family = AF_INET;
    endereco_servidor.sin_port = htons(porta);
    memcpy(&endereco_servidor.sin_addr.s_addr, servidor->h_addr, servidor->h_length);

    if(connect(socket_fd, (struct sockaddr*)&endereco_servidor, sizeof(endereco_servidor)) < 0){
        perror("Erro ao conectar");
        close(socket_fd);
        return -1;
    }

    char requisicao[TAMANHO_BUFFER];
    sprintf(requisicao, "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", caminho, host);
    
    if(send(socket_fd, requisicao, strlen(requisicao), 0) < 0){
        perror("Erro ao enviar requisição");
        close(socket_fd);
        return -1;
    }

    FILE* arquivo = fopen(nome_arquivo_saida, "wb");
    if(arquivo == NULL){
        perror("Erro ao abrir arquivo de saída");
        close(socket_fd);
        return -1;
    }

    char buffer[TAMANHO_BUFFER];
    int bytes_lidos;
    int cabecalhos_encontrados = 0;
    int sucesso = 0;

    while((bytes_lidos = read(socket_fd, buffer, TAMANHO_BUFFER - 1)) > 0){
        if(!cabecalhos_encontrados){
            char* inicio_corpo = strstr(buffer, "\r\n\r\n");
            
            if(inicio_corpo != NULL){
                cabecalhos_encontrados = 1;
                if(strncmp(buffer, "HTTP/1.1 200 OK", 15) == 0){
                    sucesso = 1;
                }else{
                    fprintf(stderr, "Erro: Servidor retornou um status não-OK\n");
                    buffer[256] = '\0'; 
                    fprintf(stderr, "Resposta do servidor:\n%s\n", buffer);
                    break;
                }
                
                inicio_corpo += 4; 
                int tamanho_corpo = bytes_lidos - (inicio_corpo - buffer);
                fwrite(inicio_corpo, 1, tamanho_corpo, arquivo);
            }
        }else{
            fwrite(buffer, 1, bytes_lidos, arquivo);
        }
    }

    if(bytes_lidos < 0){
        perror("Erro ao ler do socket");
    }

    fclose(arquivo);
    close(socket_fd);

    if(!sucesso){
        remove(nome_arquivo_saida);
        return -1;
    }

    printf("Arquivo '%s' baixado com sucesso.\n", nome_arquivo_saida);
    return 0;
}