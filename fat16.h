#define ATTR_DIRECTORY 0x10
#define FAT16_EOC 0xFFF8

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

    unsigned char BPB_FATSz32; //? bytes
    //nomes es posa per evitar errors. ja savem que no hi haura fat32.
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

unsigned short fat16SeguentCluster(BPB bpb, int fd, unsigned short cluster) {
    unsigned short seguent = FAT16_EOC;
    unsigned int fatOffset = bpb.BPB_RsvdSecCnt * bpb.BPB_BytsPerSec;

    lseek(fd, fatOffset + ((unsigned long)cluster * 2), SEEK_SET);
    read(fd, &seguent, sizeof(unsigned short));

    return seguent;
}

int fat16ClusterValid(unsigned short cluster) {
    return cluster >= 2 && cluster < FAT16_EOC;
}

// Validacio consistent del BPB. La signatura 0xAA55 nomes diu que el sector
// d'arrencada es valid, pero pot apareixer tambe a FAT12, FAT32, NTFS o un
// MBR. Segons l'especificacio oficial de Microsoft, l'unica manera correcta
// de determinar el tipus de FAT es calcular el nombre de clusters de dades:
// FAT16 ha de tenir entre 4085 i 65524 clusters.
int comprovarSiEsFat16Correcte(BPB bpb) {

    unsigned int TotalSectors = 0;
    unsigned int fatSize = 0;
    if (bpb.BPB_TotSec16 != 0){
        TotalSectors = bpb.BPB_TotSec16;
    } else {
        TotalSectors = bpb.BPB_TotSec32;
    }
    if (bpb.BPB_FATSz16 != 0){
        fatSize = bpb.BPB_FATSz16;
    } else {
        fatSize = bpb.BPB_FATSz32;
    }

    unsigned int RootDirSectors = ((bpb.BPB_RootEntCnt * 32) + bpb.BPB_BytsPerSec - 1) / bpb.BPB_BytsPerSec;
    unsigned int NonDataSectors = bpb.BPB_RsvdSecCnt + ( bpb.BPB_NumFATs * fatSize ) + RootDirSectors;

    unsigned int DataSectors = TotalSectors - NonDataSectors;

    unsigned int TotalClusters = DataSectors / bpb.BPB_SecPerClus;

    if (TotalClusters < 4085)
        return 0; //fat12
    else if (TotalClusters < 65525)
        return 1; //fat16
    else
        return 0; //fat32
}

void printFilesystemInfoFat16(BPB bpb) {

    printf("\n");
    printf("\n");
    printf("------ Filesystem Information ------\n");
    printf("\n");
    printf("\n");
    printf("Filesystem: FAT16\n");
    printf("\n");

    char *buf = generateStringWithZeroAtEnd (bpb.BS_OEMName, sizeof(bpb.BS_OEMName));
    printf("System name: %s\n", buf);
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

char *fatNameAString(unsigned char nomFat[11]) {

    int pos = 0;
    char *nomString = (char *) malloc (13);

    int finalNom = 7;
    while (finalNom >= 0 && nomFat[finalNom] == ' ') {
        finalNom--;
    }
    int finalExt = 10;
    while (finalExt >= 8 && nomFat[finalExt] == ' ') {
        finalExt--;
    }

    for (int i = 0; i <= finalNom; i++) {
        if ((nomFat[i] <= 'Z') && (nomFat[i] >= 'A')) {
            nomString[pos++] = nomFat[i] - 'A' + 'a';
        } else {
            nomString[pos++] = nomFat[i];
        }
    }

    if (finalExt >= 8) {
        nomString[pos++] = '.';
        for (int i = 8; i <= finalExt; i++) {
            if ((nomFat[i] <= 'Z') && (nomFat[i] >= 'A')) {
                nomString[pos++] = nomFat[i] - 'A' + 'a';
            } else {
                nomString[pos++] = nomFat[i];
            }
        }
    }
    nomString[pos] = '\0';
    return nomString;
}

int fat16EsPunt(RootDir dr) {
    return (dr.DIR_Attr & ATTR_DIRECTORY) && dr.DIR_Name[0] == '.' &&
        (dr.DIR_Name[1] == ' ' || (dr.DIR_Name[1] == '.' && dr.DIR_Name[2] == ' '));
}

void treeFat16Recursiu (BPB bpb, 
    int fd, 
    unsigned int dataSectionBegin,
    unsigned int Offset,
    unsigned int midaDir,
    int nivell,
    int* columnes) {

    RootDir dr;
    RootDir dr_next;
    unsigned int inici = Offset;

    while (Offset < inici + midaDir) {

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
        if (fat16EsPunt(dr)) {
            continue;
        }

        for (int i = 0; i < nivell; i++) {
            if (columnes[i]){
                printf("│   ");
            }
            else{ 
                printf("    ");
            }
        }

        while (dr_next.DIR_Name[0] == 0xE5){ //el seguent es arxiu borrat
            read(fd, &dr_next, sizeof(RootDir)); //seguent
        }

        char *nomReal = fatNameAString(dr.DIR_Name);
        if (dr_next.DIR_Name[0] == 0x00){
            //si es el ultim de la carpeta
            printf("└── %s\n", nomReal);
            columnes[nivell] = 0;
        } else {
            printf("├── %s\n", nomReal);
        }
        free(nomReal);

        if (dr.DIR_Attr & ATTR_DIRECTORY) {
            unsigned short cluster = dr.DIR_FstClusLO;
            unsigned int clusterSize = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec;
            columnes[nivell + 1] = 1; 
            while (fat16ClusterValid(cluster)) {
                unsigned int iba = dataSectionBegin + ((cluster - 2) * bpb.BPB_SecPerClus);
                unsigned int fileOffset = iba * bpb.BPB_BytsPerSec;
                treeFat16Recursiu (bpb, fd, dataSectionBegin, fileOffset, clusterSize, nivell + 1, columnes);
                cluster = fat16SeguentCluster(bpb, fd, cluster);
            }
        } 
    }
}

void treeFat16 (BPB bpb, int fd) {

    unsigned int fileAllocationTables_size = bpb.BPB_NumFATs * bpb.BPB_FATSz16;
    unsigned int RootDirSectors = ((bpb.BPB_RootEntCnt * 32) + bpb.BPB_BytsPerSec - 1) / bpb.BPB_BytsPerSec;
    unsigned int dataSectionBegin = bpb.BPB_RsvdSecCnt + fileAllocationTables_size + RootDirSectors;
    unsigned int RootDirOffset = (bpb.BPB_RsvdSecCnt + fileAllocationTables_size) * bpb.BPB_BytsPerSec;

    int columnes[32];
    for(int i = 0; i < 32; i++) {
        columnes[i] = 0;
    }
    columnes[0] = 1;

    printf(".\n");
    treeFat16Recursiu(bpb, fd, dataSectionBegin, RootDirOffset, bpb.BPB_RootEntCnt * 32, 0, columnes);
}

void llegirFitxerFat16 (int fd, BPB bpb, unsigned int dataSectionBegin, RootDir dr){

        unsigned char *cluster = malloc (bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec);
        unsigned short currentCluster = dr.DIR_FstClusLO;

        unsigned int bytesRestants = dr.DIR_FileSize;

        while (bytesRestants > 0 && fat16ClusterValid(currentCluster)) {

            unsigned int iba = dataSectionBegin + ((currentCluster - 2) * bpb.BPB_SecPerClus);
            unsigned int fileOffset = iba * bpb.BPB_BytsPerSec;
            unsigned int clusterSize = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec;
            unsigned int bytesALlegir = (bytesRestants < clusterSize) ? bytesRestants : clusterSize;
            lseek(fd, fileOffset, SEEK_SET);
            read(fd, cluster, bytesALlegir);

            fwrite(cluster, 1, bytesALlegir, stdout); //cat del cluster de fitxer, nomes bytes reals

            bytesRestants -= bytesALlegir;
            currentCluster = fat16SeguentCluster(bpb, fd, currentCluster);

        }
        free(cluster);
}

void buscarFixerFat16 (BPB bpb, 
    int fd, 
    unsigned int dataSectionBegin,
    unsigned int Offset,
    unsigned int midaDir,
    char *fitxer,
    char *arxiuTrobat) {

    RootDir dr;
    unsigned int inici = Offset;
    unsigned int clusterSize = bpb.BPB_SecPerClus * bpb.BPB_BytsPerSec;

    while (Offset < inici + midaDir) {

        lseek(fd, Offset, SEEK_SET);
        read(fd, &dr, sizeof(RootDir));
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
        if (fat16EsPunt(dr)) {
            continue;
        }

        if (dr.DIR_Attr & ATTR_DIRECTORY) {
            unsigned short cluster = dr.DIR_FstClusLO;
            while (fat16ClusterValid(cluster)) {
                unsigned int iba = dataSectionBegin + ((cluster - 2) * bpb.BPB_SecPerClus);
                unsigned int fileOffset = iba * bpb.BPB_BytsPerSec;
                buscarFixerFat16 (bpb, fd, dataSectionBegin, fileOffset, clusterSize, fitxer, arxiuTrobat);
                if (*arxiuTrobat) {
                    return;
                }
                cluster = fat16SeguentCluster(bpb, fd, cluster);
            }
        } else {
            char *nouNom = fatNameAString(dr.DIR_Name);
            if (strcmp(nouNom, fitxer) == 0){
                llegirFitxerFat16(fd, bpb, dataSectionBegin, dr);
                (*arxiuTrobat) = 1;
            }
            free(nouNom);
        }
    }
}

char *nouNomFitxer (char *fitxer, int fitLen){

    char *newNom = malloc (fitLen + 1);

    for (int i = 0; i < fitLen; i++){
        if ((fitxer[i] <= 'z') && (fitxer[i] >= 'a')) {
            newNom[i] = (fitxer[i] - 'a' + 'A');
        } else {
            newNom[i] = fitxer[i];
        }
    }
    newNom[fitLen] = '\0';

    return newNom;
}

void catFat16 (BPB bpb, int fd, char *fitxer){

    unsigned int fileAllocationTables_size = bpb.BPB_NumFATs * bpb.BPB_FATSz16;
    unsigned int RootDirSectors = ((bpb.BPB_RootEntCnt * 32) + bpb.BPB_BytsPerSec - 1) / bpb.BPB_BytsPerSec;
    unsigned int dataSectionBegin = bpb.BPB_RsvdSecCnt + fileAllocationTables_size + RootDirSectors;
    unsigned int RootDirOffset = (bpb.BPB_RsvdSecCnt + fileAllocationTables_size) * bpb.BPB_BytsPerSec;

    char *fixerNom = generateStringWithZeroAtEnd ((unsigned char *) fitxer, strlen(fitxer));
    char arxiuTrobat = 0;
    buscarFixerFat16(bpb, fd, dataSectionBegin, RootDirOffset, bpb.BPB_RootEntCnt * 32, fixerNom, &arxiuTrobat);

    if (!arxiuTrobat){
        printf("No s'ha trobat el fitxer.\n");
    }
    free(fixerNom);
}
        /*

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