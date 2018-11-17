/* 
 * Universidade Federal de Santa Catarina
 * TCP SERVER: Arquitetura de Sistemas Operacionais
 * Ale Chaito - RA: 14205353
 */

#include "lib.cpp"


int main() {
    //create_disk();
    char *path = "/home/chaito/arq.txt";
    char *content = "abdce";
    
    //int resp = write_dir(path, content);

    while(1){
        pthread_create(&tId[0], NULL, &pipe_read_dir, NULL);
        //pthread_create(&tId[1], NULL, pipeReaderFile, NULL);

        pthread_join(tId[0], NULL);
        //pthread_join(tId[1], NULL);
    }

    return 1;
};

void *pipe_read_dir(void* args) {
    
    int fd;
    char *path = "/home/chaito/arq.txt";
    char buffer[1024];


    /* opem, read and display the message from the FIFO */
    fd = open(path, O_RDONLY);
    cout << fd << endl;
    
    read(fd, buffer, 1024);
    cout << buffer << endl;
    if(strlen(buffer) != 0){
        printf("[+] Path: %s\n", buffer);
        if(write_dir(path, buffer) == 1){
            printf("Directory created successfully!\n");
        } else {
            printf("Error while trying to create directory!\n");
        }
        memset(buffer,0,strlen(buffer));
        close(fd);
    }

    return NULL;
}
