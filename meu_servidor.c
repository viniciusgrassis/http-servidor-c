#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server_socket.h"
#include "http_handler.h"

int main(int argc, char const* argv[]){
    if(argc != 2){
        fprintf(stderr, "Uso: %s <pasta_para_servir>\n", argv[0]);
        fprintf(stderr, "Exemplo: %s /home/vini1/teste\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    const char* diretorio_base = argv[1];

    int servidor_fd = configurarSocketServidor();
    
    printf("Servidor pronto, servindo a pasta '%s'...\n", diretorio_base);
    printf("Acesse em seu navegador: http://localhost:%d/\n", PORTA);

    struct sockaddr_in endereco;
    int tamanho_endereco = sizeof(endereco);

    while(1){
        int novo_socket = accept(servidor_fd, (struct sockaddr*)&endereco, (socklen_t*)&tamanho_endereco);
        if(novo_socket < 0){
            perror("accept");
            continue; 
        }

        tratarConexao(novo_socket, diretorio_base);
    }
    return 0;
}