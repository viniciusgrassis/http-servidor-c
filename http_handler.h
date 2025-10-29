#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#define TAMANHO_BUFFER 1024
#define TAMANHO_MAX_CAMINHO 512

void tratarConexao(int socket_cliente, const char* diretorio_base);

#endif