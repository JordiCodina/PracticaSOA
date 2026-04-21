#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "basic_func.h"
#include "ext2.h"
#include "fat16.h"

void printErrorFileSystem() {

    printf("ERROR: Unknown file system\n");
}

//TREE EXT2

int main (int argc, char *argv[]){

    /*
    printf("sizeof(unsigned int): (%lu) (ha de ser 4)\n", sizeof(unsigned int));
    printf("sizeof(unsigned short): (%lu) (ha de ser 2)\n", sizeof(unsigned short));
    printf("sizeof(unsigned char): (%lu) (ha de ser 1)\n", sizeof(unsigned char));
    printf("sizeof(BPB): (%lu) (ha de ser 62)\n", sizeof(BPB));
    */

    if (argc != 3) {
        printf("Incorrect arguments: /fsutils --command <file system>\n");
        return 1; 
    }  

    //hem de distiguir si el format del sistema de fitxers és FAT16 o EXT2
    //nomes pot ser 1 dels 2
    int fd = open(argv[2], O_RDONLY);

    if (fd < 0) {
        perror(argv[2]);
        return 1;
    }

    SuperBlock sb;
    lseek(fd, 1024, SEEK_SET);
    read(fd, &sb, sizeof(SuperBlock));

    BPB bpb;
    lseek(fd, 0, SEEK_SET);
    read(fd, &bpb, sizeof(BPB));
    lseek(fd, 0, SEEK_SET);

    unsigned short fat16_confirm;
    lseek(fd, 510, SEEK_SET);
    read(fd, &fat16_confirm, sizeof(short));

    char file_system = 0;

    if (sb.s_magic == 0xEF53) { //EXT2_SUPER_MAGIC
        //el camp s.magic indica si és un sistema de fitxers Ext32.
        file_system = 1;
    } else if ((fat16_confirm & 0x0000FFFF) == 0xAA55) {
        //sector[510] equals 0x55, and sector[511] equals 0xAA
        file_system = 2;
    } else { //no s'ha detectat ni EXT2 ni FAT16
        printErrorFileSystem();
        return 1;
    }

    if (strcmp(argv[1], "--info") == 0) {
        switch (file_system){
            case 1:
            printFilesystemInfoExt2(sb);
            break;
            case 2:
            printFilesystemInfoFat16(bpb);
            break;
        }
    } else if (strcmp(argv[1], "--tree") == 0) {
        switch (file_system){
            case 1:
                treeExt2(sb,fd);
            break;
            case 2:
                treeFat16 (bpb, fd);
            break;
        }
    } else {
        printf("Incorrect arguments: /fsutils --command <file system>\n");
        close(fd);
        return 1; 
    }

    close(fd);
	return 0;
}