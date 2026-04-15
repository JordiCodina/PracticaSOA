#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>


//Ext2 superblock
typedef struct _SuperBlock {

    unsigned int s_inodes_count;        //4 bytes, number
    unsigned int s_blocks_count;        //4 bytes, number
    unsigned int s_r_blocks_count;      //4 bytes, number
    unsigned int s_free_blocks_count;   //4 bytes, number
    unsigned int s_free_inodes_count;   //4 bytes, number
    unsigned int s_first_data_block;    //4 bytes, id number of the block that contains superblock
    unsigned int s_log_block_size;      //4 bytes, block size = 1024 << s_log_block_size;
    int s_log_frag_size;                //4 bytes, can be negative value
    unsigned int s_blocks_per_group;    //4 bytes, number
    unsigned int s_frags_per_group;     //4 bytes, number
    unsigned int s_inodes_per_group;    //4 bytes, number
    unsigned int s_mtime;               //4 bytes, Unix time
    unsigned int s_wtime;               //4 bytes, Unix time

    unsigned short s_mnt_count;         //2 bytes
    unsigned short s_max_mnt_count;     //2 bytes
    unsigned short s_magic;             //2 bytes
    unsigned short s_state;             //2 bytes
    unsigned short s_errors;            //2 bytes
    unsigned short s_minor_rev_level;   //2 bytes

    unsigned int s_lastcheck;           //4 bytes, Unix time
    unsigned int s_checkinterval;       //4 bytes
    unsigned int s_creator_os;          //4 bytes
    unsigned int s_rev_level;           //4 bytes

    unsigned short s_def_resuid;        //2 bytes
    unsigned short s_def_resgid;        //2 bytes

    // EXT2_DYNAMIC_REV Specific
    unsigned int s_first_ino;           //4 bytes
    unsigned short s_inode_size;        //2 bytes
    unsigned short s_block_group_nr;    //2 bytes
    unsigned int s_feature_compat;      //4 bytes
    unsigned int s_feature_incompat;    //4 bytes
    unsigned int s_feature_ro_compat;   //4 bytes

    unsigned char s_uuid[16];           //16 bytes
    unsigned char s_volume_name[16];    //16 bytes
    unsigned char s_last_mounted[64];   //64 bytes

    unsigned int s_algo_bitmap;         //4 bytes

    // Performance Hints
    unsigned char s_prealloc_blocks;    //1byte
    unsigned char s_prealloc_dir_blocks;//1byte

    // Journaling Support
    unsigned char s_journal_uuid[16];   //16 bytes
    unsigned int s_journal_inum;        //4 bytes
    unsigned int s_journal_dev;         //4 bytes
    unsigned int s_last_orphan;         //4 bytes

    // Directory Indexing Support
    unsigned int s_hash_seed[4];        //16 bytes
    unsigned char s_def_hash_version;   //1 byte

    // Other options
    unsigned int s_default_mount_options;//4 bytes
    unsigned int s_first_meta_bg;       //4 bytes
} SuperBlock;



typedef struct {
    unsigned int bg_block_bitmap;
    unsigned int bg_inode_bitmap;
    unsigned int bg_inode_table;
    unsigned short bg_free_blocks_count;
    unsigned short bg_free_inodes_count;
    unsigned short bg_used_dirs_count;
    unsigned short bg_pad;
    unsigned char bg_reserved[12];
} ext2_blockDescriptor;



typedef struct {
    unsigned short i_mode;       
    unsigned short i_uid;         
    unsigned int i_size;          
    unsigned int i_atime;         
    unsigned int i_ctime;    
    unsigned int i_mtime;       
    unsigned int i_dtime;     
    unsigned short i_gid;       
    unsigned short i_links_count; 
    unsigned int i_blocks;      
    unsigned int i_flags;        
    unsigned int i_osd1;        

    unsigned int i_block[15];

    unsigned int i_generation;   
    unsigned int i_file_acl;   
    unsigned int i_dir_acl;      
    unsigned int i_faddr;     
    unsigned char i_osd2[12];

}ext2_inode;


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

//hack per passar de Unix Time a una cadena.
char * unixTimeToStringHack (time_t timeInt){

    struct tm timestamp; //estructura timestamp de la llibreria <time.h>
    localtime_r(&timeInt, &timestamp); //funcio de <time.h>. de int a timestamp
    char *date = asctime(&timestamp); //funcio de <time.h>. de timestamp a cadema

    return date;
}
//generear el mateix pero amb '\0' al final.
char *generateStringWithZeroAtEnd (unsigned char str[], unsigned long strSize){
    char *buf = malloc (strSize + 1);
    memcpy(buf, str, strSize);
    buf[strSize] = '\0';

    return buf;
}

void printFilesystemInfoExt2(SuperBlock sb) {

    printf("\n");
    printf("\n");
    printf("------ Filesystem Information ------\n");
    printf("\n");
    printf("\n");
    printf("Filesystem: EXT2\n");
    printf("\n");

    printf("INODE INFO\n");
    printf("\tSize: %u\n", sb.s_inode_size);
    printf("\tNum Inodes: %u\n", sb.s_inodes_count);
    printf("\tFirst Inode: %u\n", sb.s_first_ino);
    printf("\tInodes Group: %u\n", sb.s_inodes_per_group);
    printf("\tFree Inodes: %u\n", sb.s_free_inodes_count);

    printf("INFO BLOCK\n");
    printf("\tBlock size: %u\n", (1024 << sb.s_log_block_size));
    printf("\tReserved blocks: %u\n", sb.s_r_blocks_count);
    printf("\tFree blocks: %u\n", sb.s_free_blocks_count);
    printf("\tTotal blocks: %u\n", sb.s_blocks_count);
    printf("\tFirst block: %u\n", sb.s_first_data_block);
    printf("\tGroup blocks: %u\n", sb.s_blocks_per_group);
    printf("\tGroup frags: %u\n", sb.s_frags_per_group);

    printf("INFO VOLUME\n");
    char *buf = generateStringWithZeroAtEnd (sb.s_volume_name, sizeof(sb.s_volume_name));
    printf("\tVolume name: %s\n", buf);
    free(buf);

    char *date = unixTimeToStringHack ((time_t) sb.s_lastcheck);
    printf("\tLast Checked: %s", date);
    date = unixTimeToStringHack ((time_t) sb.s_mtime);
    printf("\tLast Mounted: %s", date);
    date = unixTimeToStringHack ((time_t) sb.s_wtime);
    printf("\tLast Written: %s", date);

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

int isDirectoryExt2(ext2_inode inode) {
    return (inode.i_mode & 0xF000) == 0x4000; 
    //Aquets valors estan trets de -> inode Table -> i_mode -> 
    //-> EXT2_S_IFDIR 0x4000  //es directori
    // 0xF000 es una mascara per obtenir els bits de tipus de fitxer , potser no fa falta
}

ext2_blockDescriptor readBlockDescriptor(int fd, SuperBlock sb, int group) {

    ext2_blockDescriptor blockDescriptor;
    unsigned int blockSize = 1024 << sb.s_log_block_size;

    unsigned int blockStart;
    if (blockSize == 1024) {
        blockStart = 2048;
    } else {
        blockStart = blockSize;
    }

    unsigned int blockDescriptorPos = blockStart + (group * sizeof(ext2_blockDescriptor));

    lseek(fd, blockDescriptorPos, SEEK_SET);
    int bytes_read = read(fd, &blockDescriptor, sizeof(ext2_blockDescriptor));

    if (bytes_read != sizeof(ext2_blockDescriptor)) {
        ext2_blockDescriptor empty = {0};
        return empty;
    }

    return blockDescriptor;
}

ext2_inode readInode(int fd, SuperBlock sb, unsigned int inodeNum) {
    unsigned int blockSize = 1024 << sb.s_log_block_size;
    int group = (inodeNum-1)/sb.s_inodes_per_group;
    int localIndex = (inodeNum-1)% sb.s_inodes_per_group;
    ext2_blockDescriptor bd =readBlockDescriptor(fd, sb, group);
    
    // numero de bloc * mida del bloc + index  del inode que volem mirar * mida fixa de l'inode  
    unsigned long offset =(unsigned long)bd.bg_inode_table * blockSize  + (localIndex * sb.s_inode_size);

    ext2_inode currentInode;
    lseek(fd, offset, SEEK_SET);
    read(fd, &currentInode, sizeof(ext2_inode)); 

    return currentInode;
} 


void processBlock (SuperBlock sb, int fd, unsigned int blockNum, int nivell,int* columnes) {
    unsigned int blockSize = 1024 << sb.s_log_block_size;

    unsigned long blockOffset = (unsigned long)blockNum * blockSize;//aixo es la posicio del bloc de dades del directori
        unsigned int bytesLlegits = 0;

        while (bytesLlegits<blockSize) { 
            lseek(fd, blockOffset + bytesLlegits, SEEK_SET);// Ens posicionem a l'entrada actual dins del bloc de dades
            
            unsigned int fillInode;
            unsigned short midaTotal;
            unsigned char midaName;
            char fileType;

            //aquets camps estan a dataBlocks del Block group 2
            read(fd, &fillInode, 4);//inode
            read(fd, &midaTotal, 2);//rec_len
            read(fd, &midaName, 1);//name_len
            read(fd, &fileType, 1);//file_type

             if (fillInode != 0) { //si l'inode es 0, vol dir que no hi ha cap entrada en aquesta posicio, per tant no fem res
              
                char name[256];
                read(fd, name, midaName);//llegim el nom
                name[midaName] = '\0';


               if (!(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)) { //si el nom de l'entrada es "." o ".." no el mostrem, perque no aporten informacio
                //aqui printem...    
                for (int i = 0; i < nivell; i++) {
                        if (columnes[i]){
                            printf("│   ");
                        }
                        else{ 
                            printf("    ");
                        }
                    }
                    int esUltim = (bytesLlegits + midaTotal >= blockSize);
                   if (esUltim){
                         printf("└── %s\n", name);
                   } 
                    else {
                        printf("├── %s\n", name);
                    }

                    ext2_inode fill = readInode(fd, sb, fillInode);
                    if (isDirectoryExt2(fill)) {    //si es directori llavors seguim  amb la recursivitat, si no es directori no fem res
                    columnes[nivell] = 1; 
                    treeExt2Recursiu(sb, fd, fillInode, nivell + 1, columnes);
                    columnes[nivell] = 0;                   
                    }else{


                    }
                }
            }
           if (midaTotal==0) break;

            bytesLlegits += midaTotal;
        }
         
    }

void treeExt2Recursiu (SuperBlock sb, int fd, int inodeNum, int nivell, int* elements) {

    ext2_inode currentInode = readInode(fd, sb, inodeNum);
    if (!isDirectoryExt2(currentInode)){ 
        return;
    }
    for (int i = 0; i < 12; i++) {
        if (currentInode.i_block[i] != 0) {
            processBlock(sb, fd, currentInode.i_block[i], nivell, elements);
        }
    }
    // blocs indirectes 12 13 14 ???



}


void treeExt2(SuperBlock sb, int fd) {
    printf(".\n");
    int inodeRoot=2;
    int columnes[32];
    for(int i = 0; i < 32; i++) {
        columnes[i] = 0;
    }

    treeExt2Recursiu(sb, fd, inodeRoot, 0,columnes); 
}




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
            //tree...
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