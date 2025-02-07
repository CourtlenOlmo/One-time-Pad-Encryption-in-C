#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

void findLargestFile(){
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;
    char* token;
    char* savePtr;
    int fileSize = 0;
    char* filePath;
    char fileName[250];
    char* movie = "movies";


    currDir = opendir(".");
    if (currDir == NULL) {
    perror("opendir");
    exit(EXIT_FAILURE);
    }
    printf("Directory opened successfully.\n");

    while((entry = readdir(currDir)) != NULL){
        //get the name of the file and copy it into a new string for tokenizing
        stat(entry->d_name, &dirStat);
        strncpy(fileName, entry->d_name, sizeof(fileName));
        fileName[sizeof(fileName) - 1] = '\0';
        token = strtok_r(fileName, "_.", &savePtr);

            // Debugging statements
        printf("Checking file: %s\n", entry->d_name);
        printf("Token: %s, File Size: %ld, Current Max File Size: %d\n", token, dirStat.st_size, fileSize);

        //if the filesize if bigger than the previous, and the name starts with "movies"
        if(dirStat.st_size > fileSize && strcmp(token, movie) == 0 && strstr(entry->d_name, ".csv") != NULL){
            fileSize = dirStat.st_size;
            printf("test");
        } 
    }
}

void findSmallestFile(){
    
}

void findFileByName(){
    
}

void main(){
    int selection = 0;

    do{
        printf("1. Select file to process\n"
                "2. Exit the program\n"
                "Enter a choice 1 or 2: ");
        scanf("%d", &selection);
        if(selection == 0 || selection >= 2){
            printf("You entered an incorrect choice. Try again.\n");
        }
    } while (selection == 0 || selection > 2);

    if(selection == 1){
        selection = 0;
        do{
        printf("Which file you want to process?\n"
                "Enter 1 to pick the largest file\n"
                "Enter 2 to pick the smallest file\n"
                "Enter 3 to specify the name of a file\n"
                "Enter a choice from 1 to 3: ");
        scanf("%d", &selection);

        if(selection < 1 || selection > 3){
            printf("You entered an incorrect choice. Try again.\n");
            }
        } while (selection == 0 || selection > 2);
    }
    if(selection == 1){
        findLargestFile();
    }
    else if(selection == 2){
        findSmallestFile();
    }
    else if(selection == 3){
        findFileByName();
    }
}