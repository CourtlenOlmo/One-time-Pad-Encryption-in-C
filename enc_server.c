#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  char* enc_message;

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    // Get the message from the client and display it
    memset(buffer, '\0', 256);
    // Read the client's message from the socket
    charsRead = recv(connectionSocket, buffer, 255, 0); 
    if (charsRead < 0){
      error("ERROR reading from socket");
    }

    //seperate the message into variables named fileName and Key
    char* fileName = strtok(buffer, "|");
    char* key = strtok(NULL, "|");

    // Open the key and filename files
    FILE* keyFile = fopen(key, "r");
    FILE* file = fopen(fileName, "r");

    // Assign a numerical value to the letters of the alphabet
    char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int alpha[27];
    for (int i = 0; i < 27; i++){
      alpha[i] = i;
    }

    // Initialize enc_message to be the same size as the plaintext message
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    enc_message = malloc((fileSize + 1) * sizeof(char)); // +1 for the null terminator
    if (enc_message == NULL) {
      error("ERROR allocating memory");
    }
    memset(enc_message, '\0', fileSize + 1);

    //iterate through the key and file, adding the value of each letter to the enc_message
    int enc_message_index = 0;
    while (1){
      char keyChar = fgetc(keyFile);
      char fileChar = fgetc(file);
      if (keyChar == EOF || fileChar == EOF){
        break;
      }
      int keyIndex = 0;
      int fileIndex = 0;
      for (int i = 0; i < 27; i++){
        if (keyChar == alphabet[i]){
          keyIndex = alpha[i];
        }
        if (fileChar == alphabet[i]){
          fileIndex = alpha[i];
        }
      }
      int enc_index = (keyIndex + fileIndex) % 27;
      enc_message[enc_message_index++] = alphabet[enc_index];
    }

    enc_message[enc_message_index] = '\0';

    fclose(keyFile);
    fclose(file);

    
    // Send the encoded message back to the client
    charsRead = send(connectionSocket, 
                    enc_message, strlen(enc_message), 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }

    free(enc_message);

    // Close the connection socket for this client
    close(connectionSocket); 
  }

  // Close the listening socket
  close(listenSocket); 
  return 0;
}
