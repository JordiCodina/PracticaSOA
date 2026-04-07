

#include <stdio.h>


int main (int argc, char *argv[]){
    
    if (argc != 3) {
        printf("Incorrect arguments: /fsutils --info <file system>\n");
        return 1; 
    }  
    if (strcmp(argv[1], "--info") != 0) {
        printf("Incorrect arguments: /fsutils --info <file system>\n");
        return 1; 
    }


    switch (argv[2]) {
        case "studentext100MB":
            printf("studentext100MB\n");


            break;
        case "DEVINfat":
            printf("DEVINfat\n");
            break;
        default:
            printf("Unknown file system: %s\n", argv[2]);
            return 1;

    }

	return 0;
}

void printFilesystemInfo(const char *filesystem) {
  
    printf("------ Filesystem Information ------\n");
    printf("Filesystem: %s\n", filesystem);
   
   
}
