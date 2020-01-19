#ifndef MYSOCKET_H
#define MYSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>

//o socket vai se comportar de acordo com o tipo de requisição
void *my_socket(int type);

//associa o socket criado a uma porta no sistema operacional
int my_bind(void *structSocket, const char *endpoint);

//utilizada pelo socket para receber a mensagem que o respondente vai enviar
int my_recv(void *structSocket, void *msg, int buffersize, int flag);

//utilizada para enviar mensagens para outro socket após conectado cliente-servidor
int my_send(void *structSocket, void *msg, int buffersize, int flag);

//utilizada para conectar o socket ao endereço especificado
int my_connect(void *structSocket, const char *endpoint);

//fecha a conexão daquele respectivo socket
int my_close(void *structSocket);

#endif