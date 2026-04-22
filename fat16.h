#define ATTR_DIRECTORY 0x10

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

typedef struct _RootDir {

    unsigned char DIR_Name[11];         //11 bytes
    unsigned char DIR_Attr;             //1 byte
    unsigned char DIR_NTRes;            //1 byte
    unsigned char DIR_CrtTimeTenth;     //1 byte
    unsigned short DIR_CrtTime;         //2 bytes
    unsigned short DIR_CrtDate;         //2 bytes
    unsigned short DIR_LstAccDate;      //2 bytes
    unsigned short DIR_FstClusHI;       //2 bytes
    unsigned short DIR_WrtTime;         //2 bytes
    unsigned short DIR_WrtDate;         //2 bytes
    unsigned short DIR_FstClusLO;       //2 bytes
    unsigned int DIR_FileSize;          //4 bytes

} RootDir;

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

void treeFat16Recursiu (BPB bpb, 
    int fd, 
    unsigned int dataSectionBegin,
    unsigned int Offset,
    int nivell,
    int* columnes) {

    RootDir dr;
    RootDir dr_next;

    while (1) {

        lseek(fd, Offset, SEEK_SET);
        read(fd, &dr, sizeof(RootDir));
        read(fd, &dr_next, sizeof(RootDir)); //seguent
        Offset = Offset + 32;

        if (dr.DIR_Name[0] == 0x00){
            //ja no hi ha més arxius
            return;
        }
        if (dr.DIR_Name[0] == 0xE5){
            //arxiu borrat
            continue;
        }
        if ((dr.DIR_Attr & 0x0F) == 0x0F) {
            continue; // saltar LFN long file name
        }
        if (dr.DIR_Attr & ATTR_DIRECTORY) {
            // ignorar les carpetes "." y ".."
            if (dr.DIR_Name[0] == '.') {
                continue;
            }
        }

        for (int i = 0; i < nivell; i++) {
            if (columnes[i]){
                printf("│   ");
            }
            else{ 
                printf("    ");
            }
        }

        if (dr_next.DIR_Name[0] == 0x00){
            //si es el ultim de la carpeta
            printf("└── %s\n", dr.DIR_Name);
            columnes[nivell] = 0;
        } else {
            printf("├── %s\n", dr.DIR_Name);
        }

        if (dr.DIR_Attr & ATTR_DIRECTORY) {
            unsigned int iba = dataSectionBegin + ((dr.DIR_FstClusLO - 2) * bpb.BPB_SecPerClus);
            unsigned int fileOffset = iba * bpb.BPB_BytsPerSec;
            columnes[nivell + 1] = 1; 
            treeFat16Recursiu (bpb, fd, dataSectionBegin, fileOffset, nivell + 1, columnes);
        } 
    }
}

void treeFat16 (BPB bpb, int fd) {

    //fat 16 structure:
    //reserved | file allocation tables | Root Dir | Data

    //reserved -> bpb.BPB_RsvdSecCnt (hauria de ser 1) in SECTORS
    //printf("bpb.BPB_RsvdSecCnt: %u\n", bpb.BPB_RsvdSecCnt);

    //file allocation tables -> fat count * sectors per fat in SECTORS
    unsigned int fileAllocationTables_size = bpb.BPB_NumFATs * bpb.BPB_FATSz16;
    //printf("fileAllocationTables_size: %u\n", fileAllocationTables_size);

    //Root Dir in SECTORS
    unsigned int RootDirSectors = ((bpb.BPB_RootEntCnt * 32) + bpb.BPB_BytsPerSec - 1) / bpb.BPB_BytsPerSec;
    //printf("RootDirSectors: %u\n", RootDirSectors);
    
    //data section begin in SECTORS
    unsigned int dataSectionBegin = bpb.BPB_RsvdSecCnt + fileAllocationTables_size + RootDirSectors;
    //printf("dataSectionBegin: %u\n", dataSectionBegin);

    unsigned int RootDirOffset = (bpb.BPB_RsvdSecCnt + fileAllocationTables_size) * bpb.BPB_BytsPerSec;

    int columnes[32];
    for(int i = 0; i < 32; i++) {
        columnes[i] = 0;
    }
    columnes[0] = 1;

    printf(".\n");
    treeFat16Recursiu(bpb, fd, dataSectionBegin, RootDirOffset, 0, columnes);
}

        /*
        //if we wanted to read the file
        unsigned int fileOffset = iba * bpb.BPB_BytsPerSec;
        unsigned char *cluster = malloc (bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec);
        lseek(fd, fileOffset, SEEK_SET);
        read(fd, cluster, sizeof(bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec));
        printf("cluster: [%c][%c][%c]", cluster[0], cluster[1], cluster[2]);
        free(cluster);
        */
        /*
        //then we have to look at the fat
        unsigned int FATOffset = (bpb.BPB_RsvdSecCnt) * bpb.BPB_BytsPerSec;
        //FAT16 -> each entry is 16 bits
        lseek(fd, FATOffset + (sizeof(short) * dr.DIR_FstClusLO), SEEK_SET);
        unsigned short entry_card = 0;
        while (entry_card != 65535){ //FFFF marks EOF
            read(fd, &entry_card, sizeof(unsigned short));
            printf("entry_card: %u\n", entry_card);
        }
        printf("EOF\n");
        */