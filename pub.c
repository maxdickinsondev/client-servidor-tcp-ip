#include "teste.c"
#include <time.h>

int main(void){
    printf("Aguardando clientes se conectar...\n");

    void *publisher = my_socket(PUB);
    int rc = my_bind(publisher, "tcp://*:55556");
    assert(rc == 0);

    // Inicializa um gerador aleatório de números
    time_t t;
    srand((unsigned) time(&t));

    while(1){

        int zipcode, temperature, relhumidty;
        zipcode = 10;
        temperature = 10;
        relhumidty = 10;

        // Envia mensagens aos subscreventes
        char update[20];
        sprintf(update, "%05d %d %d", zipcode, temperature, relhumidty);
        my_send(publisher, update, 20, 0);
    }

    my_close(publisher);

    return 0;

}