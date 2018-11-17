// GET A FREE INDEX FOR WRITE FILE IN MEMORY
int get_memo_file() {
    FILE *fp = fopen(DISK_NAME, "rb+");
    char* tmp = (char*) malloc(1);
    int i = BEGIN_FILES;

    while(i < END_FILES) {
        fseek(fp, i, SEEK_SET);
        fgets(tmp, 1+1, fp);
        if(strcmp(tmp, "0") == 0) {
            break;
        }
        else {
            i = i + 22;
        }
    }
    cout << i << endl;
    return i;
}

void check_inodes() {
    FILE *fp = fopen(DISK_NAME, "rb+");
    char* tmp = (char*) malloc(8);
    int i = BEGIN_INODE;

    while(i < END_INODE) {
        fseek(fp, i, SEEK_SET);
        fgets(tmp, 8, fp);
        cout << tmp << endl;
        if(strcmp(tmp, "0") == 0) {
            break;
        }
        else {
            i = i + 8;
        }
    }
}