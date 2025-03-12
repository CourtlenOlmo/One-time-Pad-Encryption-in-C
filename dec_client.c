#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

void comp_length( char* key, char* fileName){
    //compare the key against fileName, if it is shorter, print to stderr and exit program
    FILE *keyFile = fopen(key, "r");
    FILE *file = fopen(fileName, "r");
    fseek(keyFile, 0, SEEK_END);
    long keySize = ftell(keyFile);
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    if (keySize < fileSize){
      fprintf(stderr, "Error: key '%s' is too short\n", key);
      exit(1);
    }

    char invalid_chars[] = "abcdefghijklmnopqrstuvwxyz!@#$%^&*()_+{}|:<>?`~";
    //check if the file contains any lower case letters or invalid characters
    fseek(file, 0, SEEK_SET);
    char c;
    while ((c = fgetc(file)) != EOF){
      if (strchr(invalid_chars, c) != NULL){
        fprintf(stderr, "Error: input contains bad characters\n");
        exit(1);
      }
    }

    fclose(keyFile);
    fclose(file);
}

// Helper function to send data
void sendData(int socketFD, const char* buffer) {
  int totalCharsWritten = 0;
  int charsWritten;
  while (totalCharsWritten < strlen(buffer)){
    charsWritten = send(socketFD, buffer + totalCharsWritten, strlen(buffer) - totalCharsWritten, 0);
    if (charsWritten < 0){
      error("CLIENT: ERROR writing to socket");
    }
    totalCharsWritten += charsWritten;
  }
}

// Helper function to receive data
void receiveData(int socketFD, char* buffer, int bufferSize) {
  memset(buffer, '\0', bufferSize);
  int charsRead = recv(socketFD, buffer, bufferSize - 1, 0);
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
}

int main(int argc, char *argv[]) {
  int socketFD, charsRead;
  struct sockaddr_in serverAddress;
  char* fileName = argv[1];
  char* key = argv[2];
  // Check usage & args
  if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    exit(0); 
  } 

  comp_length(key, fileName);

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }

  // Send a check message to the server to identify it
  char checkMessage[] = "DEC_CLIENT_CHECK";
  sendData(socketFD, checkMessage);

  // Receive the server's response
  char response[256];
  receiveData(socketFD, response, sizeof(response));

  // Check if the server is enc_server
  if (strcmp(response, "ENC_SERVER") == 0) {
    fprintf(stderr, "CLIENT: ERROR connected to enc_server\n");
    close(socketFD);
    exit(2);
  }

  // Open the key and filename files
  FILE* keyFile = fopen(key, "r");
  FILE* file = fopen(fileName, "r");

  //put how long keyFile is into a variable
  fseek(keyFile, 0, SEEK_END);
  long keySize = ftell(keyFile);
  fseek(keyFile, 0, SEEK_SET);
  //put how long file is into a variable
  fseek(file, 0, SEEK_END);
  long fileSize = ftell(file);
  fseek(file, 0, SEEK_SET);

  //initalize buffer to be the size of the key and file
  char buffer[keySize + fileSize + 2];
  memset(buffer, '\0', sizeof(buffer));

  // Read the contents of the file into a temporary buffer
  char tempBuffer[fileSize + 1];
  fread(tempBuffer, sizeof(char), fileSize, file);
  tempBuffer[fileSize] = '\0';

  // Copy the contents of the file into the buffer, removing newline characters
  int i = 0;
  for (int j = 0; j < fileSize; j++) {
    if (tempBuffer[j] != '\n') {
      buffer[i++] = tempBuffer[j];
    }
  }
  buffer[i++] = '|';

  // Read the contents of the key file into a temporary buffer
  char tempKeyBuffer[keySize + 1];
  fread(tempKeyBuffer, sizeof(char), keySize, keyFile);
  tempKeyBuffer[keySize] = '\0';

  // Copy the contents of the keyfile into the buffer, removing newline characters
  for (int j = 0; j < keySize; j++) {
    if (tempKeyBuffer[j] != '\n') {
      buffer[i++] = tempKeyBuffer[j];
    }
  }
  buffer[i] = '\0';

  // Send message to server
  sendData(socketFD, buffer);

  // Clear out the buffer again for reuse
  memset(buffer, '\0', sizeof(buffer));

  // Read data from the socket
  receiveData(socketFD, buffer, sizeof(buffer));

  //output buffer to stdout
  printf("%s\n", buffer);

  // Close the socket
  close(socketFD); 
  return 0;
}