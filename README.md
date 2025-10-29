# Cliente e Servidor HTTP em C

Este projeto é um trabalho acadêmico que implementa um cliente e um servidor HTTP simples em C.

## Funcionalidades

### 1. Cliente (meu_navegador)
O `meu_navegador` é um cliente HTTP de linha de comando. Ele é capaz de:
-   Fazer requisições `GET` a servidores HTTP (não suporta HTTPS).
-   Analisar URLs, incluindo hosts e portas personalizadas (ex: `http://localhost:5050`).
-   Resolver o nome do host para um IP (DNS).
-   Salvar o arquivo recebido (o corpo da resposta) localmente.
-   Retornar erros em caso de falha na conexão ou status não-200 (ex: 404 Not Found).

### 2. Servidor (meu_servidor)
O `meu_servidor` é um servidor HTTP que serve arquivos de um diretório local. Ele é capaz de:
-   Lidar com requisições `GET`.
-   Servir arquivos de um diretório-base especificado.
-   Proteger contra ataques de *Path Traversal* (`..`).
-   Enviar o `Content-Type` (MIME) correto para arquivos comuns.
-   **Listagem de Diretório:** Se um diretório for requisitado e ele **não** contiver um `index.html`, o servidor retorna uma página HTML listando o conteúdo do diretório.

## Como Compilar

Este projeto usa um `Makefile` para facilitar a compilação. Basta executar o comando `make`:
make

# Exemplo: Servindo a pasta 'teste' local
./meu_servidor teste

# Exemplo 1: Baixando do seu próprio servidor local
./meu_navegador http://localhost:8080/pagina.txt

# Exemplo 2: Baixando a listagem do diretório (se não houver index.html)
./meu_navegador http://localhost:8080/

# Exemplo 3: Baixando de um site externo (apenas HTTP)
./meu_navegador [http://info.cern.ch/index.html](http://info.cern.ch/index.html)