#ifndef HTTP_UTILS_H
#define HTTP_UTILS_H

#include <stddef.h>

const char* obterNomeArquivoDaUrl(const char* url);
int analisarUrl(const char* url, char* host, char* caminho, int* porta);
const char* obterTipoConteudo(const char* caminho);

#endif