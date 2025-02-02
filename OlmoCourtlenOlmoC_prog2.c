#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct movies{
    char* name;
    int   year;
    float rating;
    char language[100];
    struct movies *next;
};

struct movies* head = NULL;
struct movies* tail = NULL;

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
    printf("\nProcessed file %s and parsed data for %d movies\n\n", filePath, movieNum);

}

void showMoviesInYear(struct movies* list, int year){
    int moviesInYear = 0;

    //Loop throught the list until the last node is found, print the name if it matches the year value
    while(list != NULL){
        if (list->year == year){
            printf("%s\n", list->name);
            moviesInYear += 1;
        }
        list = list->next;
    }
    if (moviesInYear == 0){
        printf("No data about movies released in the year %d\n", year);
    }
}

void showHighestRatedMovies(struct movies* list){
    for(int i = 1900; i <= 2021; i++){
        float highestRating = 0.0;
        char* movieTitle = "";

        struct movies* current = list;

        while(current != NULL){
            if (current->year == i){
                if (current->rating > highestRating){
                    highestRating = current->rating;
                    movieTitle = current->name;
                }
            }
            current = current->next;
        }

        if(highestRating > 0.0){
            printf("%d %.1f %s\n", i, highestRating, movieTitle);
        }
    }
}

void showMoviesByLanguage(struct movies* list, char* language){
    struct movies* current = list;
    char* token;
    char* savePtr;
    int bool = 0;

    //loop through the nodes, create a copy of the language section, and tokenize it
    while(current != NULL){
        char langCopy[100];
        strncpy(langCopy, current->language, sizeof(langCopy));
        langCopy[sizeof(langCopy) - 1] = '\0';
        token = strtok_r(langCopy, "[];", &savePtr);

        //compare our target language and the token, if they match print year and name
        while(token != NULL){
            if(strcmp(token, language) == 0){
                printf("%d %s\n", current->year, current->name);
                bool = 1;
                break;
            }
            token = strtok_r(NULL, "[];", &savePtr);
        }
        current = current->next;
    }
    //If no matches were found, print exit message
    if(bool == 0){
        printf("No data about movies released in %s\n", language);
    }
}

int main ( int argc, char **argv ){
    int selection = 0;
    int yearChoice;
    char language[21];

    if (argc < 2)
    {
    printf("You must provide the name of the file to process\n");
    printf("Example usage: ./movies movies.csv\n");
    return EXIT_FAILURE;
    }
    processMovieFile(argv[1]);

    do{
        printf("1. Show movies released in the specified year\n"
        "2. Show highest rated movie for each year\n"
        "3. Show the title and year of release of all movies in a specific language\n"
        "4. Exit from the program\n\n"
        "Enter a choice from 1 to 4: ");

        scanf("%d", &selection);

        if (selection < 1 || selection > 4 ){
            printf("You entered an incorrect choice. Try again.\n\n");
        }
        else if (selection == 1){
            printf("Enter the year for which you want to see movies: \n");
            scanf("%d", &yearChoice);
            showMoviesInYear(head, yearChoice);
        }
        else if (selection == 2){
            showHighestRatedMovies(head);
            printf("\n");
        }
        else if (selection == 3){
            printf("Enter the language for which you want to see movies: \n");
            scanf("%20s", language);
            showMoviesByLanguage(head, language);
        }
        else if (selection == 4){
        }
    } while (selection != 4);
    return EXIT_SUCCESS;
}