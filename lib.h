#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
//#include "memory.h"

#define DISK_NAME "disk.bin"
#define DISK_SIZE 1024000             //1M

#define BEGIN_FILES     0
#define END_FILES       2000      // 50B
#define BEGIN_INODE     2001
#define END_INODE       4000    // 2KB
#define BEGIN_DATA      4001
#define END_DATA        50000    // 500B
#define BEDIN_DIR       50001
#define END_DIR       60000


#define FILE_BYTE   22
#define INODE_BYTE  6
#define DATA_BYTE   100

//10B:dirn_name, 10B:dir_name_next, 2B:INODE_ID_FILE
#define DIR_BYTE    22

#define FILES BEGIN_FILES, END_FILES, FILE_BYTE
#define INODES BEGIN_INODE, END_INODE, INODE_BYTE
#define DATAS BEGIN_DATA, END_DATA, DATA_BYTE
#define DIRS BEDIN_DIR, END_DIR, DIR_BYTE

//Headers
void create_disk();
void get_file(int pos);
char* read_pos(int pos, int size);
char* gen_inode_id(int pos);
void check_memo(int start, int limit, int size);
int get_free_pos(int start, int limit, int size);
void create_inode(char* inode_id, char* content);
char* gen_id(int pos, int size);
void* pipe_read_dir(void* args);
int write_dir(char* path, char* content);

pthread_t tId[2];

using namespace std;