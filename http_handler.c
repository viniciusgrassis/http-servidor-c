#include "http_handler.h"
#include "http_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

static void enviarRespostaErro(int socket_cliente, int codigo_status, const char* mensagem_status){
    char resposta[TAMANHO_BUFFER];
    sprintf(resposta, "HTTP/1.1 %d %s\r\nContent-Length: %ld\r\n\r\n%s",
            codigo_status, mensagem_status, strlen(mensagem_status), mensagem_status);
    send(socket_cliente, resposta, strlen(resposta), 0);
}

static void enviarCabecalhosSucesso(int socket_cliente, long tamanho_conteudo, const char* tipo_conteudo){
    char cabecalhos[TAMANHO_BUFFER];
    sprintf(cabecalhos, "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nContent-Type: %s\r\n\r\n", 
            tamanho_conteudo, tipo_conteudo);
    send(socket_cliente, cabecalhos, strlen(cabecalhos), 0);
}

static void enviarConteudoArquivo(int socket_cliente, int arquivo_fd){
    char buffer_arquivo[TAMANHO_BUFFER];
    ssize_t bytes_lidos;
    while((bytes_lidos = read(arquivo_fd, buffer_arquivo, TAMANHO_BUFFER)) > 0){
        if(send(socket_cliente, buffer_arquivo, bytes_lidos, 0) < 0){
            perror("Erro ao enviar dados do arquivo");
            break; 
        }
    }
    close(arquivo_fd);
}

static void enviarListagemDiretorio(int socket_cliente, const char* caminho_diretorio, const char* caminho_requisicao){
    char corpo[TAMANHO_BUFFER * 4]; 
    char caminho_entrada[TAMANHO_MAX_CAMINHO];
    char link_entrada[TAMANHO_MAX_CAMINHO + 100];

    sprintf(corpo, "<html><head><title>Index of %s</title></head><body><h1>Index of %s</h1><hr><ul>",
            caminho_requisicao, caminho_requisicao);

    DIR* diretorio = opendir(caminho_diretorio);
    if(diretorio == NULL){
        perror("Erro ao abrir diretório para listagem");
        enviarRespostaErro(socket_cliente, 500, "Internal Server Error");
        return;
    }

    struct dirent* entrada;
    while((entrada = readdir(diretorio)) != NULL){
        if(strcmp(entrada->d_name, ".") == 0) continue; 

        char* barra = "";
        snprintf(caminho_entrada, TAMANHO_MAX_CAMINHO, "%s/%s", caminho_diretorio, entrada->d_name);
        struct stat status_entrada;
        if(stat(caminho_entrada, &status_entrada) == 0 && S_ISDIR(status_entrada.st_mode)){
            barra = "/";
        }
        
        if(strcmp(caminho_requisicao, "/") == 0){
            snprintf(link_entrada, sizeof(link_entrada), "<li><a href=\"%s%s\">%s%s</a></li>", 
                entrada->d_name, barra, entrada->d_name, barra);
        }else{
            snprintf(link_entrada, sizeof(link_entrada), "<li><a href=\"%s/%s%s\">%s%s</a></li>", 
                caminho_requisicao, entrada->d_name, barra, entrada->d_name, barra);
        }

        strcat(corpo, link_entrada);
    }
    closedir(diretorio);

    strcat(corpo, "</ul><hr></body></html>");
    enviarCabecalhosSucesso(socket_cliente, strlen(corpo), "text/html");
    send(socket_cliente, corpo, strlen(corpo), 0);
}

void tratarConexao(int socket_cliente, const char* diretorio_base){
    char buffer[TAMANHO_BUFFER] = {0};
    if(read(socket_cliente, buffer, TAMANHO_BUFFER - 1) <= 0){
        close(socket_cliente);
        return;
    }

    char metodo[16], caminho[TAMANHO_MAX_CAMINHO];
    sscanf(buffer, "%s %s", metodo, caminho);

    if(strstr(caminho, "..")){
        printf("Ataque de Path Traversal detectado: %s\n", caminho);
        enviarRespostaErro(socket_cliente, 400, "Bad Request");
        close(socket_cliente);
        return;
    }

    printf("Requisição recebida: %s %s\n", metodo, caminho);

    char caminho_arquivo[TAMANHO_MAX_CAMINHO];
    char caminho_index[TAMANHO_MAX_CAMINHO];

    snprintf(caminho_arquivo, TAMANHO_MAX_CAMINHO, "%s%s", diretorio_base, caminho);

    struct stat status_arquivo;
    if(stat(caminho_arquivo, &status_arquivo) < 0){
        printf("Arquivo não encontrado: %s\n", caminho_arquivo);
        enviarRespostaErro(socket_cliente, 404, "Not Found");
    }else if(S_ISDIR(status_arquivo.st_mode)){
        snprintf(caminho_index, TAMANHO_MAX_CAMINHO, "%s/index.html", caminho_arquivo);
        struct stat status_index;

        if(stat(caminho_index, &status_index) == 0 && S_ISREG(status_index.st_mode)){
            int arquivo_fd = open(caminho_index, O_RDONLY);
            if(arquivo_fd < 0){
                enviarRespostaErro(socket_cliente, 500, "Internal Server Error");
            }else{
                printf("Servindo index: %s (%ld bytes)\n", caminho_index, status_index.st_size);
                const char* tipo_conteudo = obterTipoConteudo(caminho_index);
                enviarCabecalhosSucesso(socket_cliente, status_index.st_size, tipo_conteudo);
                enviarConteudoArquivo(socket_cliente, arquivo_fd);
            }
        }else{
            printf("Listando diretório: %s\n", caminho_arquivo);
            enviarListagemDiretorio(socket_cliente, caminho_arquivo, caminho);
        }
    }else if(S_ISREG(status_arquivo.st_mode)){
        int arquivo_fd = open(caminho_arquivo, O_RDONLY);
        if(arquivo_fd < 0){
            enviarRespostaErro(socket_cliente, 500, "Internal Server Error");
        }else{
            printf("Servindo arquivo: %s (%ld bytes)\n", caminho_arquivo, status_arquivo.st_size);
            const char* tipo_conteudo = obterTipoConteudo(caminho_arquivo);
            enviarCabecalhosSucesso(socket_cliente, status_arquivo.st_size, tipo_conteudo);
            enviarConteudoArquivo(socket_cliente, arquivo_fd);
        }
    }
    close(socket_cliente);
}