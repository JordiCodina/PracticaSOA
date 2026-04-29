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

int main (int argc,char *argv[]){

    /*
    printf("sizeof(unsigned int): (%lu) (ha de ser 4)\n", sizeof(unsigned int));
    printf("sizeof(unsigned short): (%lu) (ha de ser 2)\n", sizeof(unsigned short));
    printf("sizeof(unsigned char): (%lu) (ha de ser 1)\n", sizeof(unsigned char));
    printf("sizeof(BPB): (%lu) (ha de ser 62)\n", sizeof(BPB));
    */

    if (argc < 3) {
        printf("Incorrect arguments: ./fsutils --command <file system>\n");
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
    if (lseek(fd, 1024, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }
    if (read(fd, &sb, sizeof(SuperBlock)) != (ssize_t) sizeof(SuperBlock)) {
        perror("read");
        close(fd);
        return 1;
    }

    BPB bpb;
    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }
    if (read(fd, &bpb, sizeof(BPB)) != (ssize_t) sizeof(BPB)) {
        perror("read");
        close(fd);
        return 1;
    }

    unsigned short fat16_confirm;
    if (lseek(fd, 510, SEEK_SET) < 0) {
        perror("lseek");
        close(fd);
        return 1;
    }
    if (read(fd, &fat16_confirm, sizeof(short)) != (ssize_t) sizeof(short)) {
        perror("read");
        close(fd);
        return 1;
    }

    char file_system = 0;

    if (sb.s_magic == 0xEF53) { //EXT2_SUPER_MAGIC
        //el camp s.magic indica si és un sistema de fitxers Ext32.
        file_system = 1;
    } else if ((fat16_confirm & 0x0000FFFF) == 0xAA55 && comprovarSiEsFat16Correcte(bpb)) {
        //sector[510]=0x55 i sector[511]=0xAA es la signatura del sector d'arrencada,
        //pero no es exclusiva de FAT16. Cal validar tambe els camps del BPB
        //(comptatge de clusters, bytes per sector, FATs, ...) per evitar
        //falsos positius amb FAT12, FAT32 o altres volums bootables.
        file_system = 2;
    } else { //no s'ha detectat ni EXT2 ni FAT16
        printErrorFileSystem();
        close(fd);
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
    } else if (strcmp(argv[1], "--cat") == 0) {
        if (argc != 4){
            printf("falta que posis el fitxer.\n");
            printf("Incorrect arguments: ./fsutils --cat <file system> <file>\n");
        } else {
            switch (file_system){
                case 1:
                    printf("es ext2, ha de ser fat16.\n");
                break;
                case 2:
                    catFat16 (bpb, fd, argv[3]);
                break;
            }
        }
    } else {
        printf("Incorrect arguments: ./fsutils --command <file system>\n");
        close(fd);
        return 1; 
    }

    close(fd);
	return 0;
}