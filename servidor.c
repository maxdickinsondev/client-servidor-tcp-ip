#include "teste.c"

int main(void){
    //type 1 == REP (respondente)
    //type 2 == REQ (requisitor)
    
    void *responder = my_socket(REP);
    int rc =  my_bind(responder, "tcp://*:55555");
    assert (rc == 0);

    while(1){
        char buffer[10];
        my_recv(responder, buffer, 10, 0);
        printf("Recebi Hello\n");
        sleep(1);
        my_send(responder, "World", 5, 0);
    }

    return 0;
}