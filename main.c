#include "ext2.h"

// hack to remove padding
#pragma pack(push, 1)

typedef struct _BPB {

    //primers 36 bytes
    unsigned char BS_jmpBoot[3];        //3 bytes
    unsigned char BS_OEMName[8];        //8 bytes
    unsigned short BPB_BytsPerSec;      //2 bytes
    unsigned char BPB_SecPerClus;       //1 byte
    unsigned short BPB_RsvdSecCnt;      //2 bytes
    unsigned char BPB_NumFATs;          //1 byte
    unsigned short BPB_RootEntCnt;      //2 bytes
    unsigned short BPB_TotSec16;        //2 bytes
    unsigned char BPB_Media;            //1 byte
    unsigned short BPB_FATSz16;         //2 bytes
    unsigned short BPB_SecPerTrk;       //2 bytes
    unsigned short BPB_NumHeads;        //2 bytes
    unsigned int BPB_HiddSec;           //4 bytes
    unsigned int BPB_TotSec32;          //4 bytes

    //Fat12 and Fat16 Structure Starting at Offset 36
    unsigned char BS_DrvNum;            //1 byte
    unsigned char BS_Reserved1;         //1 byte
    unsigned char BS_BootSig;           //1 byte
    unsigned int BS_VolID;              //4 bytes
    unsigned char BS_VolLab[11];        //11 bytes
    unsigned char BS_FilSysType[8];     //8 bytes
} BPB;

// stop hack to remove padding
#pragma pack(pop)

void printFilesystemInfoFat16(BPB bpb) {

    printf("\n");
    printf("\n");
    printf("------ Filesystem Information ------\n");
    printf("\n");
    printf("\n");
    printf("Filesystem: FAT16\n");
    printf("\n");

    char *buf = generateStringWithZeroAtEnd (bpb.BS_OEMName, sizeof(bpb.BS_OEMName));
    printf("System name: %s\n", bpb.BS_OEMName);
    free(buf);
    printf("Sector size: %u\n", bpb.BPB_BytsPerSec);
    printf("Sectors per cluster: %u\n", bpb.BPB_SecPerClus);
    printf("Reserved sectors: %u\n", bpb.BPB_RsvdSecCnt);
    printf("# of FATs: %u\n", bpb.BPB_NumFATs);
    printf("Max root entries: %u\n", bpb.BPB_RootEntCnt);
    printf("Sectors per FAT: %u\n", bpb.BPB_FATSz16);
    buf = generateStringWithZeroAtEnd (bpb.BS_VolLab, sizeof(bpb.BS_VolLab));
    printf("Label: %s\n", buf);
    free(buf);
}

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
    printf("%x\n", fat16_confirm);
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
            //tree...
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