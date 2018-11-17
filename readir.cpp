
void *pipe_read_dir(void* args) {
    int fd;
    char *path = "/home/chaito/file.txt";
    char *content = "abcde";
    char buffer[1024];
    int p[2];


    if (pipe(p) < 0) 
        exit(1); 
    
    write(p[1], path, strlen(path));
    /* continued */
    /* write pipe */
    read(p[0], buffer, strlen(path));

    if(strlen(buffer) != 0){
        cout << "[+] Path: " << buffer << endl;
        if(write_dir(buffer, content) == 1){
            cout << "Directory created successfully!" << endl;
            //check_memo(FILES);
            exit(0);
        } else {
            cout << "Error while trying to create directory!" << endl;
        }
        memset(buffer, 0, strlen(buffer));
    }
}
