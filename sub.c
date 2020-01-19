#include "teste.c"

int main(int argc, char *argv[]){
    
    printf ("Coletando atualizacoes do servidor...\n");

    void *subscriber = my_socket(SUB);
    int rc = my_connect(subscriber, "tcp://localhost:55556");
    assert(rc == 0);

    //Processa 100 updates
    int update_nbr;
    long total_temp = 0;

    for (update_nbr = 0; update_nbr < 100; update_nbr++){

        char buffer[50];
        my_recv(subscriber, buffer, 50, 0);

        int zipcode, temperature, relhumidty;
        sscanf(buffer, "%d %d %d", &zipcode, &temperature, &relhumidty);
        total_temp += temperature;

    }

    printf("Temperatura media foi %dF\n", (int)(total_temp/update_nbr));

    my_close(subscriber);

    return 0;
}