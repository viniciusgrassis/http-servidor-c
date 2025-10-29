CC = gcc
CFLAGS = -Wall -g -Wno-deprecated-declarations

CLIENTE_EXE = meu_navegador
SERVIDOR_EXE = meu_servidor

CLIENTE_OBJS = meu_navegador.o http_client.o http_utils.o
SERVIDOR_OBJS = meu_servidor.o server_socket.o http_handler.o http_utils.o

.PHONY: all
all: $(CLIENTE_EXE) $(SERVIDOR_EXE)

$(CLIENTE_EXE): $(CLIENTE_OBJS)
	@echo "Ligando (Linking) Cliente..."
	$(CC) $(CFLAGS) -o $(CLIENTE_EXE) $(CLIENTE_OBJS)

$(SERVIDOR_EXE): $(SERVIDOR_OBJS)
	@echo "Ligando (Linking) Servidor..."
	$(CC) $(CFLAGS) -o $(SERVIDOR_EXE) $(SERVIDOR_OBJS)

meu_navegador.o: meu_navegador.c http_client.h http_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

http_client.o: http_client.c http_client.h http_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

meu_servidor.o: meu_servidor.c server_socket.h http_handler.h
	$(CC) $(CFLAGS) -c $< -o $@

http_handler.o: http_handler.c http_handler.h http_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

server_socket.o: server_socket.c server_socket.h
	$(CC) $(CFLAGS) -c $< -o $@

http_utils.o: http_utils.c http_utils.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Limpando..."
	rm -f $(CLIENTE_EXE) $(SERVIDOR_EXE) *.o