#include "server_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

int configurarSocketServidor(){
    int servidor_fd;
    struct sockaddr_in endereco;
    int opt = 1;

    if((servidor_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket falhou");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(servidor_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY; 
    endereco.sin_port = htons(PORTA);

    if(bind(servidor_fd, (struct sockaddr*)&endereco, sizeof(endereco)) < 0){
        perror("bind falhou");
        exit(EXIT_FAILURE);
    }

    if(listen(servidor_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escutando na porta %d\n", PORTA);
    return servidor_fd;
}