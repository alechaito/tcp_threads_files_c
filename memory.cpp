/* 
 * Universidade Federal de Santa Catarina
 * TCP SERVER: Arquitetura de Sistemas Operacionais
 * Ale Chaito - RA: 14205353
 */

#include "lib.cpp"


int main() {
    create_disk();

    while(1){
        pthread_create(&tId[0], NULL, &pipe_read_dir, NULL);

        pthread_join(tId[0], NULL);

    }

    return 1;
};
