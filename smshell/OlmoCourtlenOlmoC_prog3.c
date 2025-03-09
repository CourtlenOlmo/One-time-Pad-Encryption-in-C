#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct movies{
    char* name;
    int   year;
    float rating;
    char language[100];
    struct movies *next;
};

struct movies* head = NULL;
struct movies* tail = NULL;

//This function was adapted from Module Three
struct movies* createMovie(char* name, int year, char* language, float rating){

    struct movies* currMovie = malloc(sizeof(struct movies));

    currMovie->name = calloc(strlen(name) + 1, sizeof(char));
    strcpy(currMovie->name, name);

    currMovie->year = year;
    currMovie->rating = rating;

    strncpy(currMovie->language, language, sizeof(currMovie->language));

    currMovie->next = NULL;
    return currMovie;
}


void createDirectory(struct movies* list){
    unsigned int seed = time(0);
    int randomNumber = rand_r(&seed) % 99999;
    char directoryName[50];
    DIR* currDir;
    char fileName[100];

    //create a directory named OlmoC.movies.randomNumber with permissions set to rwxr-x---
    sprintf(directoryName, "olmoc.movies.%d", randomNumber);
    mkdir(directoryName, 0750);
    printf("Created directory with name %s\n", directoryName);

    //in the new directory, loop through the linked list, and find all movies that were released in the same year
    for(int i = 1900; i <= 2021; i++){
        struct movies* current = list;
        while(current != NULL){
            if (current->year == i){
                sprintf(fileName, "%s/%d.txt", directoryName, i);
                FILE* file = fopen(fileName, "a");
                chmod(fileName, 0640);
                fprintf(file, "%s\n", current->name);
                fclose(file);
            }
            current = current->next;
        }
    }
}

void processMovieFile(char* filePath){
    char *currLine = NULL;
    size_t len = 0;
    int movieNum = 0;
    char* token;
    char* savePtr;

    // Open the specified file
    FILE *movieFile = fopen(filePath, "r+");

    // Read the file line by line
    while(getline(&currLine, &len, movieFile) != -1)
    {
        if (movieNum > 0 ){
            //Skip the header, then add values to name, year, rating, and language variables using strtok_r
            char name[100] = "";
            int year = 0;
            char language[100] = "";
            float rating = 0.0f;

            token = strtok_r(currLine, ",", &savePtr);
            strncpy(name, token, sizeof(name));

            token = strtok_r(NULL, ",", &savePtr);
            year = atoi(token);

            token = strtok_r(NULL, ",", &savePtr);
            strncpy(language, token, sizeof(language));

            token = strtok_r(NULL, ",", &savePtr);
            rating = strtof(token, NULL);

            struct movies* newElem = createMovie(name, year, language, rating);

            //Add the newly created movie to the linked list
            if (head == NULL) {
                head = newElem;
                tail = newElem;
            } else {
                tail->next = newElem;
                tail = newElem;
            }
        }
        movieNum += 1;
    }
    //Subract the header from the total number of movies
    movieNum -= 1;
    // Free the memory allocated by getline for currLine
    free(currLine);

    // Close the file
    fclose(movieFile);
}

void findLargestFile(){
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;
    int fileSize = 0;
    char* filePath;
    char fileName[250];


    currDir = opendir(".");

    while((entry = readdir(currDir)) != NULL){
        stat(entry->d_name, &dirStat);

        //if the filesize if bigger than the previous, and the name starts with "movies", and the file is a .csv
        if(dirStat.st_size > fileSize && strstr(entry->d_name, "movies_") != NULL && strstr(entry->d_name, ".csv") != NULL){
            fileSize = dirStat.st_size;
            strcpy(fileName, entry->d_name);
        } 
    }
    closedir(currDir);
    printf("Now processing the chosen file named %s\n", fileName);
    processMovieFile(fileName);
    createDirectory(head);
    printf("\n");
}

void findSmallestFile(){
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;
    int fileSize = 9999999;
    char* filePath;
    char fileName[250];

    currDir = opendir(".");

    while((entry = readdir(currDir)) != NULL){
        stat(entry->d_name, &dirStat);

        //if the filesize if smaller than the previous, and the name starts with "movies", and the file is a .csv
        if(dirStat.st_size < fileSize && strstr(entry->d_name, "movies_") != NULL && strstr(entry->d_name, ".csv") != NULL){
            fileSize = dirStat.st_size;
            strcpy(fileName, entry->d_name);
        }

    }
    closedir(currDir);
    printf("Now processing the chosen file named %s\n", fileName);
    processMovieFile(fileName);
    createDirectory(head);
    printf("\n");
}

int findFileByName(){
    char fileName[250];
    DIR* currDir;
    struct dirent *entry;
    struct stat dirStat;
    int fileFound = 0;

    printf("Enter the complete file name: ");
    scanf("%s", fileName);
    currDir = opendir("."); 

    while((entry = readdir(currDir)) != NULL){
        stat(entry->d_name, &dirStat);

        //if the name of the file matches the user input - process file, if a matching file is not found, write an error message
        if(strcmp(entry->d_name, fileName) == 0){
            printf("Now processing the chosen file named %s\n", fileName);
            processMovieFile(fileName);
            createDirectory(head);
            fileFound = 1;
            closedir(currDir);
            return 1;
        }
    }
    if(fileFound == 0){
        printf("The file %s was not found. Try again.\n", fileName);
        closedir(currDir);
    }
}

void main(){
    int selectionOne = 0;
    int selectionTwo = 0;

    do{
        printf("1. Select file to process\n"
                "2. Exit the program\n"
                "Enter a choice 1 or 2: ");
        scanf("%d", &selectionOne);

        if(selectionOne == 0 || selectionOne > 2){
            printf("You entered an incorrect choice. Try again.\n");
        }

        if(selectionOne == 1){
            do{
            printf("\n");
            printf("Which file you want to process?\n"
                    "Enter 1 to pick the largest file\n"
                    "Enter 2 to pick the smallest file\n"
                    "Enter 3 to specify the name of a file\n"
                    "Enter a choice from 1 to 3: ");
            scanf("%d", &selectionTwo);

            if(selectionTwo < 1 || selectionTwo > 3){
                printf("You entered an incorrect choice. Try again.\n");
                }

                if(selectionTwo == 1){
                    findLargestFile();
                }
                else if(selectionTwo == 2){
                    findSmallestFile();
                }
                else if(selectionTwo == 3){
                    //If a file is found, this function returns a 1 to return to the main menu
                    //Otherwise it runs this section again
                    selectionTwo = findFileByName();
                }
            } while (selectionTwo == 0 || selectionTwo > 3);
        }
    } while (selectionOne != 2);
}