#include <stdio.h>
#include "http_utils.h"
#include "http_client.h"

int main(int argc, char* argv[]){
    if(argc != 2){
        fprintf(stderr, "Uso: %s http://host/caminho/do/arquivo\n", argv[0]);
        return 1;
    }

    char host[256];
    char caminho[1024];
    int porta = 80;
    
    if(analisarUrl(argv[1], host, caminho, &porta) != 0){
        return 1;
    }

    printf("Debug -> Host: '%s', Porta: %d, Caminho: '%s'\n", host, porta, caminho);

    const char* nome_arquivo_saida = obterNomeArquivoDaUrl(caminho);

    if(executarHttpGet(host, porta, caminho, nome_arquivo_saida) != 0){
        fprintf(stderr, "Falha ao baixar o arquivo.\n");
        return 1;
    }

    return 0;
}