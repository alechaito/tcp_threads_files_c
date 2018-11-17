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
#include <vector>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string.h>

#define BEGIN_DIR 0
#define END_DIR 340000
#define BEGIN_INODE 340001
#define END_INODE 640000
#define BEGIN_DATA 640001
#define END_DATA 1024000

#define DISK_NAME "disk.bin"

using namespace std;

//Headers
void create_disk();
void write_disk();
void get_file(int pos);

class Control {
    public:
    int pos;
    string name;

    Control(int pos1, string name1) { 
        pos = pos1; 
        name = name1; 
    } 

    void write() {
        FILE *f = fopen(DISK_NAME, "wb");
        fseek(f, 0, SEEK_SET);
        fwrite (this, sizeof(this), 1, f);
        fclose(f);
    }

    void read(int pos) {
        Control* buffer = (Control*) malloc (sizeof(Control));
        FILE *f = fopen(DISK_NAME, "rb");
        fseek(f, pos, SEEK_SET);  
        fread (buffer, sizeof(this), 1, f);
        cout << this->pos << endl;
        cout << this->name << endl;
    }

    int check_free_pos() {
        FILE *fp;
        fp = fopen(DISK_NAME, "rb");

        Control* buffer = (Control*) malloc (sizeof(Control));

        for(int i = BEGIN_DIR; i < END_DIR; i++) {
            fseek(fp, i, SEEK_SET);        
            fread(buffer, sizeof(buffer), 1, fp);
            cout << buffer << endl;
            cout << buffer->pos << endl;
            if(i == 5) {
                break;
            }
        }
    }

};
