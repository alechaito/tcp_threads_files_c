#include "lib.h"
#include "readir.cpp"

char* write_file(char* name, char* content) {
    //cout << "[+] Write file function loading..." << endl;
    FILE *fp = fopen(DISK_NAME, "rb+");

    int pos = get_free_pos(FILES);
    char* inode_id = gen_id(pos, INODE_BYTE);
    create_inode(inode_id, content);

    fseek(fp, pos, SEEK_SET);
    fputs(name, fp);

    // Node id is stored in the last 2 bytes from 22 bytes;
    fseek(fp, pos+20, SEEK_SET);
    fputs(inode_id, fp);
    
    fclose(fp);
    return inode_id;
}

//10B:dirn_name, 10B:dir_name_next, 2B:INODE_ID_FILE
int write_dir(char* path, char* content) {
    FILE *fp = fopen(DISK_NAME, "rb+");
    char *dir1, *dir2, *file, *tmp;
    int i = 0;
    
    // PREPARING DIRECTORY
    path = strdup(path);
    while( (tmp = strsep(&path,"/")) != NULL ) {
        if(i == 1) {
            dir1 = tmp;
        }
        if(i == 2) {
            dir2 = tmp;
        }
        if(i == 3) {
            file = tmp;
        }
        i = i + 1;
    }

    char* inode_id = write_file(file, content);
    
    int pos = get_free_pos(DIRS);

    // Write name
    fseek(fp, pos, SEEK_SET);
    fputs(dir1, fp);

    // Write next dir
    fseek(fp, pos+10, SEEK_SET);
    fputs(dir2, fp);

    // Write inode id
    fseek(fp, pos+20, SEEK_SET);
    fputs(inode_id, fp);


    // Erro que nao consegui corrigir
    fseek(fp, pos+20, SEEK_SET);
    fputs(inode_id, fp);
    return 1;
}


/*
INODE_DATAVEC
blockdata_id:2bytes/inode_id1:2bytes/free_space:2bytes
*/
void create_inode(char* inode_id, char* content) {
    //cout << "[+] Create inode function loading..." << endl;
    //cout << "INODE ID PASSADO POR PARAM: " << inode_id << endl;
    FILE *fp = fopen(DISK_NAME, "rb+");

    int pos = get_free_pos(INODES);
    int pos_block = get_free_pos(DATAS);
    char* block_id = gen_id(pos_block, DATA_BYTE);

    //cout << "POS DO INODE: " << pos << endl;
    //cout << "POS DO BLOCK: " << pos_block << endl;
    //cout << "BLOCO ID GERADO: " << block_id << endl;

    // Writing block id in inode
    fseek(fp, pos, SEEK_SET);
    fputs(block_id, fp);

    // Writing inode id in inode
    fseek(fp, pos+2, SEEK_SET);
    fputs(inode_id, fp);

    // Writing inode free space block
    // 100-2 from ID BLOCK
    fseek(fp, pos+4, SEEK_SET);
    fputs("98", fp);

    cout << "[+] Free space for block memo associated with inode is 98 bytes." << endl;

     // Writing inode id in inode
    fseek(fp, pos+2, SEEK_SET);
    fputs(inode_id, fp);


    // Writing block id in block data
    fseek(fp, pos_block, SEEK_SET);
    fputs(block_id, fp);

    // Writing content file in block data
    fseek(fp, pos_block+2, SEEK_SET);
    fputs(content, fp);

    int size = strlen(content);
    int total = 98 - size;
    cout << "[+] Content consumed " << size << " bytes from block memo, free: " << (total) << endl;

    char *str = (char*) malloc(2);
    if(total <= 9) {
        sprintf(str, "0%d", total);
    } else {
        sprintf(str, "%d", total);
    }
    cout << str << endl;
    // Writing in inode the free space of the block memo
    fseek(fp, pos+4, SEEK_SET);
    fputs(str, fp);

    fclose(fp);
}



// GET A FREE INDEX FOR WRITE FILE IN MEMORY
int get_free_pos(int start, int limit, int size) {
    //cout << "[+] Get free pos function loading..." << endl;
    FILE *fp = fopen(DISK_NAME, "rb+");
    char* tmp = (char*) malloc(1);
    int i = start;

    while(i < limit) {
        fseek(fp, i, SEEK_SET);
        fgets(tmp, 1+1, fp);
        if(strcmp(tmp, "0") == 0) {
            break;
        }
        else {
            i = i + size; // INODE HAVE 8 bytes
        }
    }
    //cout << "[+] FREE MEMORY FIND: " << i << endl;
    return i;
}

// CALCULATE THE NEXT ID FOR NODE
char* gen_id(int pos, int size) {
    //cout << "[+] Gen block id function loading..." << endl;
    char *string = (char*) malloc(2);

    pos = (pos/size)+1;

    // If node id is lower than 9 he receive a 0 to completo the node idx
    if(pos <= 9) {
        sprintf(string, "0%d", pos);
    } else {
        sprintf(string, "%d", pos);
    }

    //cout << "[+] New block memo id generated: " << string << endl;
    return string;
    //cout << "No space available for new file." << endl;
}

// Reading memory content
void check_memo(int start, int limit, int size) {
    FILE *fp = fopen(DISK_NAME, "rb+");
    char* tmp = (char*) malloc(size+1);
    int i = start;
    int c = 0;

    while(i < limit) {
        fseek(fp, i, SEEK_SET);
        fgets(tmp, size+1, fp);
        cout << tmp << endl;
        if(strcmp(tmp, "0") == 0 or c == 5) {
            break;
        }
        else {
            i = i + size;
            c = c + 1;
        }
    }
}


/*//Posicao de 23 em 23
byte_pos - filename - inode_id
0 - file.txt - 01  (filename+inode_id = 23 bytes)
23 - file.txt - 02  (filename+inode_id = 23 bytes)
//-----------------*/
char* get_inode_id(int pos) {
    char* buffer = (char*) malloc(2);
    FILE *fp = fopen(DISK_NAME, "rb+");
    fseek(fp, pos+20, SEEK_SET);  
    fgets(buffer, 3, fp);
    return buffer;
}


char* read_pos(int pos, int size) {
    char* buffer = (char*) malloc(size);
    FILE *fp = fopen(DISK_NAME, "rb+");
    fseek(fp, pos, SEEK_SET);  
    fgets(buffer, size+1, fp);
    cout << buffer << endl;
    return buffer;
}

void create_disk() {
    FILE *f = fopen(DISK_NAME, "wb+");
    fseek(f, 0, SEEK_SET);        
    for(int i=0; i <= DISK_SIZE; i++)
        fputs("0", f);
    fclose(f);
    cout << "[+] Disk created sucefully. Size:1M." << endl;
}
