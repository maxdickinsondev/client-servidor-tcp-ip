#include "teste.c"

int main(void){
    //type 1 == REP (respondente)
    //type 2 == REQ (requisitor)
    
    printf("Conectando ao servidor ...\n");

    void *requester = my_socket(REQ);
    my_connect(requester, "tcp://localhost:55555");

    int request_nbr;

    for (request_nbr = 0; request_nbr != 10; request_nbr++){
        char buffer[10];
        printf("Enviando Hello %d ...\n", request_nbr);
        my_send(requester, "Hello", 5, 0);
        my_recv(requester, buffer, 10, 0);
        printf("Recebeu World %d\n", request_nbr);
    }

    my_close(requester);

    return 0;
}