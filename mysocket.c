#include "mysocket.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define REP 1
#define REQ 2
#define PUB 3
#define SUB 4

typedef struct{
    int protocol;
    int sock;
    int type; //guarda o tipo do socket criado, 1 - servidor, 2 - cliente

    int retAccept; //variável que guarda o retorno do accept, para passar no send
    int turn; //variável para determinar se será ou não enviado uma mensagem;
}Socket;

Socket structSocket[4]; //cada posição do vetor vai conter uma struct, onde vai ter o socket, tipo etc...

int sock1;
int sock2;
int sock3;
int sock4;
int aux = 0;

void *my_socket(int type){

    switch(type){
        case 1: //REP - servidor
            structSocket[1].sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //socket do REP
            structSocket[1].type = 1; 
            structSocket[1].retAccept = -1; //variável para guardar o resultado do accept do REP
            structSocket[1].turn = 0; //variável que indica se uma função foi chamada ou não
            break;
        case 2: //REQ - cliente
            structSocket[2].sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            structSocket[2].type = 2;
            structSocket[2].retAccept = -1;
            structSocket[2].turn = 0;
            break;
        case 3: //PUB - servidor
            structSocket[3].sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            structSocket[3].type = 1;
            structSocket[3].retAccept = -1;
            structSocket[3].turn = 0;
            break;
        case 4: //SUB - cliente
            structSocket[4].sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
            structSocket[4].type = 2;
            structSocket[4].retAccept = -1;
            structSocket[4].turn = 0;
            break;
        default:
            printf("SOCKET: tipo invalido...\n");
            break;
    }
    

    if (sock1 < 0 || sock2 < 0 || sock3 < 0 || sock4 < 0){
        printf("my_socket() falhou.\n");
        exit(EXIT_SUCCESS);
    }

    return &structSocket;
}

int my_bind(void *structSocket, const char *endpoint){

    //variáveis que conterão o tipo de socket criado, 1 - REP, 2 - REQ, 3 - PUB, 4 - SUB
    sock1 = ((Socket*)structSocket)[1].sock;
    sock2 = ((Socket*)structSocket)[2].sock;
    sock3 = ((Socket*)structSocket)[3].sock;
    sock4 = ((Socket*)structSocket)[4].sock;

    int rtn;

    in_port_t porta;

    //método para obter a porta passado por parâmetro
    char addrPort[50];
    strcpy(addrPort, endpoint);
    char *port = strrchr(addrPort, ':');
    *port++;

    struct sockaddr_in servAddr;
    struct sockaddr_in clntAddr;
    
    socklen_t clntAddrLen;
    clntAddrLen = sizeof(clntAddr);

    //servidor - REP
    if (((Socket*)structSocket)[1].type == 1){
        porta = atoi(port);

        memset(&servAddr, 0, sizeof(servAddr));

        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(porta);

        rtn = bind(sock1, (struct sockaddr*)&servAddr, sizeof(servAddr));

        if (rtn < 0){
            printf("my_bind_servidor_rep falhou.\n");
            exit(EXIT_SUCCESS);
        }

        if (listen(sock1, 1) < 0){
            printf("my_listen_servidor_rep falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return rtn;

    //servidor - PUB
    }else if (((Socket*)structSocket)[3].type == 1){
        
        porta = atoi(port);

        memset(&servAddr, 0, sizeof(servAddr));

        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(porta);

        rtn = bind(sock3, (struct sockaddr*)&servAddr, sizeof(servAddr));

        if (rtn < 0){
            printf("my_bind_servidor_pub falhou.\n");
            exit(EXIT_SUCCESS);
        }

        if (listen(sock3, 1) < 0){
            printf("my_listen_servidor_pub falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return rtn;
    }
}

int my_recv(void *structSocket, void *msg, int buffersize, int flag){

    sock1 = ((Socket*)structSocket)[1].sock;
    sock2 = ((Socket*)structSocket)[2].sock;
    sock3 = ((Socket*)structSocket)[3].sock;
    sock4 = ((Socket*)structSocket)[4].sock;

    ssize_t recebe;

    struct sockaddr_in clntAddr;
    
    socklen_t clntAddrLen;
    clntAddrLen = sizeof(clntAddr);

    //servidor - REP
    if (((Socket*)structSocket)[1].type == 1){

        if (((Socket*)structSocket)[1].retAccept == -1){
            //faz o accept
            printf("Aguardando clientes...\n");
            //armazena em retAccept o resultado do accept com o socket do servidor
            ((Socket*)structSocket)[1].retAccept = accept(sock1, (struct sockaddr *)&clntAddr, &clntAddrLen);

            if (((Socket*)structSocket)[1].retAccept < 0){
                printf("my_accept()_servidor_rep falhou.\n");
                exit(EXIT_SUCCESS);
            }
        }

        //faz o recv
        ((Socket*)structSocket)[1].turn = 1; //se entrou aqui, RECV do servidor REP foi chamado, então turn = 1.

        recebe = recv(((Socket*)structSocket)[1].retAccept, (char*)msg, buffersize, flag);

        if (recebe < 0){
            printf("my_send()_servidor falhou.\n");
            exit(EXIT_SUCCESS);
        }else if(recebe == 0){
            ((Socket*)structSocket)[1].retAccept = -1;
            close(((Socket*)structSocket)[1].retAccept);
            return -1;
        }

        return (int)recebe;

    //cliente - REQ
    }else if (((Socket*)structSocket)[2].type == 2){

        //verifica se o cliente chamou o my_send(), ou seja, se enviou algo, se sim então ele pode receber
        if (((Socket*)structSocket)[2].turn == 1){
            recebe = recv(sock2, (char*)msg, buffersize, flag);

            if (recebe < 0){
                printf("my_recv()_cliente_req falhou.\n");
                exit(EXIT_SUCCESS);
            }

            ((Socket*)structSocket)[2].turn = 0;

            return (int)recebe;
        }else{
            printf("Ops! pro cliente RECEBER tem que ENVIAR primeiro.\n");
            exit(1);
        }
        //cliente - SUB   
    }else if (((Socket*)structSocket)[4].type == 2){

        recebe = recv(sock4, (char*)msg, buffersize, flag);

        if (recebe < 0){
            printf("my_recv()_cliente_sub falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return (int)recebe;
    }
}

//accept auxiliar que será utilizado apenas para o PUB aceitar os múltiplos clientes
void *my_accept(){

    sock3 = ((Socket*)structSocket)[3].sock;

    struct sockaddr_in clntAddr;
    
    socklen_t clntAddrLen;
    clntAddrLen = sizeof(clntAddr);

    ((Socket*)structSocket)[3].retAccept = accept(sock3, (struct sockaddr *)&clntAddr, &clntAddrLen);

    if (((Socket*)structSocket)[3].retAccept < 0){
        printf("my_accept()_servidor falhou.\n");
        exit(EXIT_SUCCESS);
    }
}

int my_send(void *structSocket, void *msg, int buffersize, int flag){

    sock1 = ((Socket*)structSocket)[1].sock;
    sock2 = ((Socket*)structSocket)[2].sock;
    sock3 = ((Socket*)structSocket)[3].sock;

    ssize_t envia;

    pthread_t t1;

    //servidor - REP
    if (((Socket*)structSocket)[1].type == 1){

        if (((Socket*)structSocket)[1].retAccept != -1){

            //se o servidor RECEBEU então pode ENVIAR
            if (((Socket*)structSocket)[1].turn = 1){

                envia = send(((Socket*)structSocket)[1].retAccept, (char*)msg, buffersize, flag);

                //seta a variável turn do servidor REP como 0
                ((Socket*)structSocket)[1].turn = 0;

                if (envia < 0){
                    printf("my_send()_servidor_rep falhou.\n");
                }

                return (int)envia;
            }else{
                printf("Ops! pro servidor ENVIAR tem que RECEBER primeiro.\n");
                exit(1);
            }
            
        }else{
            printf("Ops! pro servidor enviar tem de receber primeiro...\n");
        }

    //cliente - REQ
    }else if (((Socket*)structSocket)[2].type == 2){

        envia = send(sock2, (char*)msg, sizeof(buffersize), flag);

        //se o cliente chamar o my_send() entratá aqui, logo turn recebe 1.
        //em seguida verifico no recv do REP se essa variável é 1, se sim então o REP pode receber...
        ((Socket*)structSocket)[2].turn = 1;

        if (envia < 0){
            printf("my_send()_cliente_req falhou.\n");
        }

        return (int)envia;
    //servidor - PUB    
    }else if (((Socket*)structSocket)[3].type == 1){

        //cria uma thread que aceita várias conexões com os clientes que forem chegando
        pthread_create(&t1, NULL, &my_accept, NULL);
        pthread_join(t1, NULL);

        sleep(1); //finge espera
        printf("Enviando dados...\n");
        sleep(4); //finge espera       

        envia = send(((Socket*)structSocket)[3].retAccept, (char*)msg, buffersize, flag);

        printf("Dados enviados...\n");

        ((Socket*)structSocket)[3].turn = 1;

        if (envia < 0){
            printf("my_send()_servidor falhou.\n");
        }

        close(((Socket*)structSocket)[3].retAccept);

        return (int)envia;
    }
}

int my_connect(void *structSocket, const char *endpoint){

    sock2 = ((Socket*)structSocket)[2].sock;
    sock4 = ((Socket*)structSocket)[4].sock;

    in_port_t porta;
    const char *endereco;
    char addrPort[50];
    strcpy(addrPort, endpoint);
    char *port = strrchr(addrPort, ':');
    *port++;

    struct sockaddr_in servAddr;

    int value;
    int rtn; 

    //cliente - REQ
    if (((Socket*)structSocket)[2].type == 2){
        porta = atoi(port);
        char *endereco = "127.0.0.1";

        memset(&servAddr, 0, sizeof(servAddr));

        servAddr.sin_family = AF_INET;
    
        value = inet_pton(AF_INET, endereco, &servAddr.sin_addr.s_addr);

        if (value == 0){
            printf("inet_pton(): A de string de endereco esta invalida.\n");
        }else if(value < 0){
            printf("inet_pton() falhou.\n");
        }

        servAddr.sin_port = htons(porta);

        rtn = connect(sock2, (struct sockaddr *) &servAddr, sizeof(servAddr));

        if (rtn < 0){
            printf("my_connect()_cliente_req falhou.\n");
            exit(EXIT_SUCCESS);
        }   

        return rtn;
    //cliente - REP
    }else if (((Socket*)structSocket)[4].type == 2){
        porta = atoi(port);
        char *endereco = "127.0.0.1";

        memset(&servAddr, 0, sizeof(servAddr));

        servAddr.sin_family = AF_INET;
    
        value = inet_pton(AF_INET, endereco, &servAddr.sin_addr.s_addr);

        if (value == 0){
            printf("inet_pton(): A string de endereco esta invalida.\n");
        }else if(value < 0){
            printf("inet_pton() falhou.\n");
        }

        servAddr.sin_port = htons(porta);

        rtn = connect(sock4, (struct sockaddr *) &servAddr, sizeof(servAddr));

        if (rtn < 0){
            printf("my_connect()_cliente_sub falhou.\n");
            exit(EXIT_SUCCESS);
        }

        printf("Conexao estabelecida com sucesso...\n");

        sleep(3);   

        return rtn;
    }
}

int my_close(void *structSocket){

    int rtn;

    sock1 = ((Socket*)structSocket)[1].sock;
    sock2 = ((Socket*)structSocket)[2].sock;
    sock3 = ((Socket*)structSocket)[3].sock;
    sock4 = ((Socket*)structSocket)[4].sock;

    //fecha conexão do REP
    if (((Socket*)structSocket)[1].type == 1){
        rtn = close(sock1);

        if (rtn < 0){
            printf("my_close()_servidor_rep falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return rtn;
    //fecha conexão do REQ
    }else if (((Socket*)structSocket)[2].type == 2){
        rtn = close(sock2);

        if (rtn < 0){
            printf("my_close()_cliente_req falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return rtn;
    //fecha conexão do PUB
    }else if (((Socket*)structSocket)[3].type == 1){
        rtn = close(sock3);

        if (rtn < 0){
            printf("my_close()_servidor_pub falhou.\n");
            exit(EXIT_SUCCESS);
        }

        return rtn;
    //fecha conexão do SUB
    }else if (((Socket*)structSocket)[4].type == 2){
        rtn = close(sock4);

        if (rtn < 0){
            printf("my_close()_cliente_sub falhou.\n");
            exit(EXIT_SUCCESS);
        }
    }
}


