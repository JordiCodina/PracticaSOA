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

void treeFat16 (BPB bpb, int fd) {

    printf("hola q tal %d %d\n", fd, bpb.BPB_BytsPerSec);
    
    int RootDirSectors = ((bpb.BPB_RootEntCnt * 32) + (bpb.BPB_BytsPerSec - 1)) / bpb.BPB_BytsPerSec;

    printf("BPB_FATSz16: %d\n", bpb.BPB_FATSz16);

    int FirstDataSector = bpb.BPB_RsvdSecCnt + (bpb.BPB_NumFATs * bpb.BPB_FATSz16) + RootDirSectors;

    int DataSec = bpb.BPB_TotSec16 - (bpb.BPB_RsvdSecCnt + (bpb.BPB_NumFATs * bpb.BPB_FATSz16) + RootDirSectors);

    int CountofClusters = (int) floor( (float) DataSec /  (float) bpb.BPB_SecPerClus);

    printf("RootDirSectors: %d, FirstDataSector: %d\n", RootDirSectors, FirstDataSector);


    if(CountofClusters < 4085) {
    /* Volume is FAT12 */
        printf("ERROR\n");
        return;
    } else if(CountofClusters < 65525) {
        /* Volume is FAT16 */
        printf("Efectivament\n");
    } else {
        /* Volume is FAT32 */
        printf("ERROR\n");
        return;
    }

    int cluster_n = 10; //per exemple

    int FATOffset = cluster_n * 2;

    int ThisFATSecNum = bpb.BPB_RsvdSecCnt + (FATOffset / bpb.BPB_BytsPerSec);
    int ThisFATEntOffset = FATOffset % bpb.BPB_BytsPerSec;

    #define WORD unsigned short
    #define DWORD unsigned int

    unsigned char SecBuff [128];

    int FAT16ClusEntryVal = *((WORD *) &SecBuff[ThisFATEntOffset]);

    *((WORD *) &SecBuff[ThisFATEntOffset]) = FAT16ClusEntryVal;

    printf("SecBuff: %s", SecBuff);
}