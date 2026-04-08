#include <stdio.h>

#define NONE 0
#define EXT2 1
#define FAT16 2

//Ext2 superblock
typedef struct _SuperBlock {

    unsigned int s_inodes_count;        //4 bytes
    unsigned int s_blocks_count;        //4 bytes
    unsigned int s_r_blocks_count;      //4 bytes
    unsigned int s_free_blocks_count;   //4 bytes
    unsigned int s_free_inodes_count;   //4 bytes
    unsigned int s_first_data_block;    //4 bytes
    unsigned int s_log_block_size;      //4 bytes
    unsigned int s_log_frag_size;       //4 bytes
    unsigned int s_blocks_per_group;    //4 bytes
    unsigned int s_frags_per_group;     //4 bytes
    unsigned int s_inodes_per_group;    //4 bytes
    unsigned int s_mtime;               //4 bytes
    unsigned int s_wtime;               //4 bytes

    unsigned short s_mnt_count;         //2 bytes
    unsigned short s_max_mnt_count;     //2 bytes
    unsigned short s_magic;             //2 bytes
    unsigned short s_state;             //2 bytes
    unsigned short s_errors;            //2 bytes
    unsigned short s_minor_rev_level;   //2 bytes

    unsigned int s_lastcheck;           //4 bytes
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

    unsigned char[16] s_uuid;           //16 bytes
    unsigned char[16] s_volume_name;    //16 bytes
    unsigned char[64] s_last_mounted;   //64 bytes

    unsigned int s_algo_bitmap;         //4 bytes

    // Performance Hints
    unsigned char s_prealloc_blocks;    //1byte
    unsigned char s_prealloc_dir_blocks;//1byte

    // Journaling Support
    unsigned char[16] s_journal_uuid;   //16 bytes
    unsigned int s_journal_inum;        //4 bytes
    unsigned int s_journal_dev;         //4 bytes
    unsigned int s_last_orphan;         //4 bytes

    // Directory Indexing Support
    unsigned int[4] s_hash_seed;        //16 bytes
    unsigned char s_def_hash_version;   //1 byte

    // Other options
    unsigned int s_default_mount_options;//4 bytes
    unsigned int s_first_meta_bg;       //4 bytes
} SuperBlock;

int main (int argc, char *argv[]){
    
    if (argc != 3) {
        printf("Incorrect arguments: /fsutils --info <file system>\n");
        return 1; 
    }  
    if (strcmp(argv[1], "--info") != 0) {
        printf("Incorrect arguments: /fsutils --info <file system>\n");
        return 1; 
    }


    char file_system = NONE;

    switch (argv[2]) {
        case "studentext100MB":
            printf("studentext100MB\n");


            break;
        case "DEVINfat":
            printf("DEVINfat\n");
            break;
        case "lolext":
            file_system = EXT2;
            char *file = open("lolext");

            char *offset = 0;

            offset += 1024;

            SuperBlock sb;
            sb.s_inodes_count = (unsigned int) file[offset];
            offset += 4;
            sb.s_blocks_count = (unsigned int) file[offset];
            offset += 4;
            sb.s_r_blocks_count = (unsigned int) file[offset];
            offset += 4;
            sb.s_free_blocks_count = (unsigned int) file[offset];
            offset += 4;
            sb.s_free_inodes_count = (unsigned int) file[offset];
            offset += 4;
            sb.s_first_data_block = (unsigned int) file[offset];
            offset += 4;
            sb.s_log_block_size = (unsigned int) file[offset];
            offset += 4;
            sb.s_log_frag_size = (unsigned int) file[offset];
            offset += 4;
            sb.s_log_frag_size = (unsigned int) file[offset];
            offset += 4;
            sb.s_blocks_per_group = (unsigned int) file[offset];
            offset += 4;
            sb.s_frags_per_group = (unsigned int) file[offset];
            offset += 4;
            sb.s_inodes_per_group = (unsigned int) file[offset];
            offset += 4;
            sb.s_mtime = (unsigned int) file[offset];
            offset += 4;
            sb.s_wtime = (unsigned int) file[offset];
            offset += 4;
            sb.s_mnt_count = (unsigned short) file[offset];
            offset += 2;
            sb.s_max_mnt_count = (unsigned short) file[offset];
            offset += 2;
            sb.s_magic = (unsigned short) file[offset];
            offset += 2;
            sb.s_state = (unsigned short) file[offset];
            offset += 2;
            sb.s_errors = (unsigned short) file[offset];
            offset += 2;
            sb.s_minor_rev_level = (unsigned short) file[offset];
            offset += 2;
            sb.s_lastcheck = (unsigned int) file[offset];
            offset += 4;
            sb.s_checkinterval = (unsigned int) file[offset];
            offset += 4;
            sb.s_creator_os = (unsigned int) file[offset];
            offset += 4;
            sb.s_rev_level = (unsigned int) file[offset];
            offset += 4;
            sb.s_def_resuid = (unsigned short) file[offset];
            offset += 2;
            sb.s_def_resgid = (unsigned short) file[offset];
            offset += 2;
            sb.s_first_ino = (unsigned int) file[offset];
            offset += 4;
            sb.s_inode_size = (unsigned short) file[offset];
            offset += 2;
            sb.s_block_group_nr = (unsigned short) file[offset];
            offset += 2;
            sb.s_feature_compat = (unsigned int) file[offset];
            offset += 4;
            sb.s_feature_incompat = (unsigned int) file[offset];
            offset += 4;
            sb.s_feature_ro_compat = (unsigned int) file[offset];
            offset += 4;
            for (int i = 0; i < 16; i++){
                sb.s_feature_ro_compat[i] = file[offset++];
            }
            for (int i = 0; i < 16; i++){
                sb.s_volume_name[i] = file[offset++];
            }
            for (int i = 0; i < 64; i++){
                sb.s_last_mounted[i] = file[offset++];
            }
            sb.s_algo_bitmap = (unsigned int) file[offset];
            offset += 4;
            sb.s_prealloc_blocks = (unsigned char) file[offset];
            offset ++;
            sb.s_prealloc_dir_blocks = (unsigned char) file[offset];
            offset ++;
            for (int i = 0; i < 16; i++){
                sb.s_journal_uuid[i] = file[offset++];
            }
            sb.s_journal_inum = (unsigned int) file[offset];
            offset += 4;
            sb.s_journal_dev = (unsigned int) file[offset];
            offset += 4;
            sb.s_last_orphan = (unsigned int) file[offset];
            offset += 4;
            for (int i = 0; 4 < 16; i++){
                sb.s_hash_seed[i] = (unsigned int) file[offset];
                offset += 4;
            }
            sb.s_def_hash_version = (unsigned char) file[offset];
            offset ++;
            sb.s_default_mount_options = (unsigned int) file[offset];
            offset += 4;
            sb.s_first_meta_bg = (unsigned int) file[offset];
            offset += 4;
        break;
        default:
            printf("Unknown file system: %s\n", argv[2]);
            return 1;

    }

    switch (file_system){
        case EXT2:
            printFilesystemInfo(sb);
        break;
        case FAT16:
        break;
    }
	return 0;
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
    printf("\tSize: ???\n");
    printf("\tNum Inodes: ???\n");
    printf("\tFirst Inode: ???\n");
    printf("\tInodes Group: ???\n");
    printf("\tFree Inodes: ???\n");

    printf("INODE BLOCK\n");
    printf("\tBlock size: ???\n");
    printf("\tReserved blocks: ???\n");
    printf("\tFree blocks: ???\n");
    printf("\tTotal blocks: ???\n");
    printf("\tFirst block: ???\n");
    printf("\tGroup blocks: ???\n");
    printf("\tGroup frags: ???\n");

    printf("INFO VOLUME\n");
    printf("\tVolume name: ???\n");
    printf("\tLast Checked: ???\n");
    printf("\tLast Mounted: ???\n");
    printf("\tLast Written: ???\n");
}

void printFilesystemInfo(const char *filesystem) {
  
    printf("------ Filesystem Information ------\n");
    printf("Filesystem: %s\n", filesystem);
   
   
}
