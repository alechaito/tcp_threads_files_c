/* 
 * Universidade Federal de Santa Catarina
 * TCP SERVER: Arquitetura de Sistemas Operacionais
 * Ale Chaito - RA: 14205353
 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <vector>

#define BUFSIZE 1024;

using namespace std;

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
        fprintf(stderr, "[+] Use mode: %s <porta>\n", argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);


    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) { 
        cout << "ERROR opening socket" << endl;
    }
    optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, 
            (const void *)&optval , sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(server_sock, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) { 
        cout << "ERROR on bind" << endl;
    }
    if (listen(server_sock, 10) < 0) { /* allow 5 requests to queue up */ 
        cout << "ERROR on listen" << endl;
    }

    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
    pthread_mutex_init(&lock, NULL);

    while(1) {
        pthread_arg = (pthread_arg_t *)malloc(sizeof *pthread_arg);
        client_address_len = sizeof pthread_arg->client_address;
        client_sock = accept(server_sock, (struct sockaddr *)&pthread_arg->client_address, &client_address_len);
        puts("[+] Connection accepted...");
        pthread_arg->sock = client_sock;
        pthread_create(&pthread, &pthread_attr, routine, (void *)pthread_arg);
    }
    pthread_join(pthread, NULL);
    pthread_mutex_destroy(&lock);
}


void *routine(void *arg) {
    //Get the socket descriptor
    pthread_arg_t *pthread_arg = (pthread_arg_t *)arg;
    int sock = pthread_arg->sock;
    struct sockaddr_in client_address = pthread_arg->client_address;
    int size_msg;
    char c_msg[2000];
    string cmd; // String de comandos auxiliar
    string result; // Resultado para os clients
    int FLAG_CD, FLAG_RM, FLAG_NANO = 0; // Flag cd
    DIR *path = opendir("."); // Diretorio aberto
    string dir = "."; // Diretorio atual
    string file_name = "";
    vector<string> _stack;
    
     
    //Send some messages to the client
    result = "[+] You have been connected, welcome.... \n";
    write(sock , result.c_str(), result.length());
    bzero(c_msg, 2001);

    //Receive a message from client
    while(1) {
        size_msg = recv(sock , c_msg, 2000, 0);
        // Parsing msg from client and remove characters
        cmd = string(c_msg);
        cmd.erase(cmd.length()-1);

        // 3 - Complemento CD
        if(FLAG_CD == 1) {
            dir = cmd;
            path = opendir(cmd.c_str());
            result = "[+] Updated directory: "+cmd+"\n";
            write(sock , result.c_str(), result.length());
            FLAG_CD = 0;
        }
        // 2 - Complemento RM
        if(FLAG_RM == 1) {
            rmdir(cmd.c_str());
            result = "[+] Removed directory: "+cmd+"\n";
            write(sock , result.c_str(), result.length());
            FLAG_RM = 0;
        }
        // 3 - Complemento NANO
        if (FLAG_NANO == 1) {
            fstream fs;
            string file_path = dir+"/new.txt";
            fs.open(file_path.c_str(), fstream::in | fstream::out | fstream::app);
            fs << cmd;
            fs.close();
            FLAG_NANO = 0;
            pthread_mutex_unlock(&lock);
        }
        /*
        1 - criar (sub)diretório
        2 - remover (sub)diretório
        3 - entrar em (sub)diretório
        4 - mostrar conteúdo do diretório
        5 - criar arquivo 
        6 - remover arquivo
        7 - escrever um sequência de caracteres em um arquivo
        9 - mostrar conteúdo do arquivo
        */
        // 1 - Criar subdiretorio
        if (cmd.compare("mkdir") == 0) {
            string create = dir+"/test";
            if(mkdir(create.c_str(), S_IRWXU != 0))
                cout << "[+] Error creating folder. \n";
            result = "[+] Directory successfully created ->" + create + "\n";
            write(sock , result.c_str(), result.length());
        }
        // 2 - Apagar subdiretorio
        else if (cmd.compare("rmdir") == 0) {
            result = "[+] Enter the name of the directory to be removed: "+cmd+"\n";
            write(sock , result.c_str(), result.length());
            FLAG_RM = 1;
        }
        // 3 - Entrar em diretorio
        else if (cmd.compare("cd") == 0) {
            result = "[+] Please enter a directory. \n"; 
            write(sock , result.c_str(), result.length());
            FLAG_CD = 1;
        }
        // 4 - Listar arquivos em um diretorio
        else if (cmd.compare("ls") == 0) {
            DIR* dirp = opendir(dir.c_str());
            struct dirent *de;

            if(dirp == NULL)
                cout << "error" << endl;

            while ((de = readdir(dirp)) != NULL) {
                result = string(de->d_name)+"\n"; 
                write(sock , result.c_str(), result.length());
            }
            //closedir(dirp);
        }
        // 5 - Criar arquivo
        else if (cmd.compare("file") == 0) {
            FILE *fp;
            string create = dir+"/new.txt";
            fp = fopen(create.c_str(), "w");
            result = "[+] File successfully created ->" + create + "\n";
            write(sock , result.c_str(), result.length());
        }
        // 5 - Remover arquivo
        else if (cmd.compare("rmfile") == 0) {
            FILE *fp;
            string create = dir+"/new.txt";
            if(remove(create.c_str()) != 0) {
                result = "[+] Error deleting file. \n";
                write(sock , result.c_str(), result.length());
            }
            result = "[+] File successfully deleted. \n";
            write(sock , result.c_str(), result.length());
        }
        // 6 - Inserir texto no arquivo
        else if (cmd.compare("nano") == 0) {
            pthread_mutex_lock(&lock);
            //_stack.push_back("filename")
            result = "[+] Enter the text to be inserted into the file: \n";
            write(sock , result.c_str(), result.length());
            FLAG_NANO = 1;
            /*int test = 100000;
            while(test >= 1) { 
            }*/
        }
        // 7 - Mostrar conteudo do arquivo
        else if (cmd.compare("cat") == 0) {
            //pthread_mutex_lock(&lock);
            ifstream file;
            string output;
            string locale = dir+"/new.txt";
            file.open(locale.c_str());
            if (file.is_open()) {
                while (!file.eof()) {
                    file >> output;
                    write(sock, output.c_str(), output.length());
                }
                result = "\n";
                write(sock , result.c_str(), result.length());
            }
            file.close();
            //pthread_mutex_unlock(&lock);
        }
        else if (cmd.compare("exit") == 0) {
            cout << "[+] Closing socket..." << endl;
            close(sock);
        }
        // Comando desconhecido
        else {
            cout << "[+] Unknown command..." << endl;
        }

        bzero(c_msg, 2001);
    }
    return 0;
}



