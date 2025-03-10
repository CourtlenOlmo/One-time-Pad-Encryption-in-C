#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
    if (argc != 2) {
        fprintf(stderr, "You must enter a key length\n");
        return 1;
    }

    int keyLength = atoi(argv[1]);
    char key[80000];
    char keyChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for (int i = 0; i < keyLength; i++){
        key[i] = keyChars[rand() % 27];
    }
    
    key[keyLength] = '\0';

    fprintf(stdout, "%s\n", key);

    return 0;
}