/* 
 * tcpserver.c - A simple TCP echo server 
 * usage: tcpserver <port>
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFSIZE 1024

// Headers
void *routine(void *socket_desc);

// Global
pthread_mutex_t lock;

typedef struct pthread_arg_t {
    int sock;
    struct sockaddr_in client_address;
} pthread_arg_t;

void error(char *msg) {
    perror(msg);
    exit(1);
}


int main(int argc, char **argv) {
    int portno, server_sock, client_sock, clientlen, optval;
    struct sockaddr_in clientaddr; /* client addr */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in address;
    char *hostaddrp; /* dotted decimal host addr string */
    struct hostent *hostp; /* client host info */
    socklen_t client_address_len;
    pthread_attr_t pthread_attr;
    pthread_arg_t *pthread_arg;
    pthread_t pthread;

    /* 
    * check command line arguments 
    */
    if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
    }
    portno = atoi(argv[1]);


    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) 
        error("ERROR opening socket");
    optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(server_sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) 
        error("ERROR on binding");

    if (listen(server_sock, 10) < 0) /* allow 5 requests to queue up */ 
        error("ERROR on listen");

    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);

    while(1) {
        pthread_arg = (pthread_arg_t *)malloc(sizeof *pthread_arg);
        client_address_len = sizeof pthread_arg->client_address;
        client_sock = accept(server_sock, (struct sockaddr *)&pthread_arg->client_address, &client_address_len);
        puts("[+] Connection accepted...");
        pthread_arg->sock = client_sock;
        pthread_create(&pthread, &pthread_attr, routine, (void *)pthread_arg);
    }

}


void *routine(void *arg) {
    //Get the socket descriptor
    pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
    int sock = pthread_arg->sock;
    struct sockaddr_in client_address = pthread_arg->client_address;
    int read_size;
    char *message , client_message[2000];
    DIR *path = opendir(".");
    char *source;
    int FLAG_CD = 0;
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ){
        //end of string marker
		//client_message[read_size] = '\0';

        if(FLAG_CD == 1) {
            printf("to aqui \n");
            source = (char*)malloc(read_size * sizeof(char));
            strcpy(source, client_message);
            strtok(source, "\n");
            path = opendir(source);
            printf("src: %s", source);
            FLAG_CD = 0;
        }
        
        client_message[read_size] = '\0';

        if( memcmp(client_message, "mkdir", 5) == 0) {
            mkdir("./new", S_IRWXU);
        }
        else if( memcmp(client_message, "cd", 2) == 0) {
            FLAG_CD = 1;
        }
        else if( memcmp(client_message, "list", 4) == 0) {
            struct dirent *de;
            while (de = readdir(path)) {
                strcat(client_message, de->d_name); 
                strcat(client_message, "\n");
                write(sock , client_message , strlen(client_message));
            }
            //closedir(path);
        }
        else{
            printf("[+] Comando desconhecido...\n");
        }

		//Send the message back to client
        write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
         
    return 0;
}




