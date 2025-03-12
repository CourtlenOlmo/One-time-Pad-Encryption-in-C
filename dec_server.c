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

// Helper function to send data
void sendData(int socketFD, const char* buffer) {
  int totalCharsWritten = 0;
  int charsWritten;
  while (totalCharsWritten < strlen(buffer)){
    charsWritten = send(socketFD, buffer + totalCharsWritten, strlen(buffer) - totalCharsWritten, 0);
    if (charsWritten < 0){
      error("SERVER: ERROR writing to socket");
    }
    totalCharsWritten += charsWritten;
  }
}

// Helper function to receive data
void receiveData(int socketFD, char* buffer, int bufferSize) {
  memset(buffer, '\0', bufferSize);
  int charsRead = recv(socketFD, buffer, bufferSize - 1, 0);
  if (charsRead < 0){
    error("SERVER: ERROR reading from socket");
  }
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char mainBuffer[80000];
  char fileBuffer[80000];
  char keyBuffer[80000];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);
  char* dec_message;

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

  // Start listening for connections. Allow up to 5 connections to queue up
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

    char con_check[256];
    receiveData(connectionSocket, con_check, sizeof(con_check));

    //send a response to the client to let them know they are connected to the right server
    char* response = "DEC_SERVER";
    sendData(connectionSocket, response);

    // Get the message from the client and display it
    receiveData(connectionSocket, mainBuffer, sizeof(mainBuffer));
    
    //separate the key and file from the buffer and put them into their own buffers
    char* token = strtok(mainBuffer, "|");
    strcpy(fileBuffer, token);
    token = strtok(NULL, "|");
    strcpy(keyBuffer, token);

    // Assign a numerical value to the letters of the alphabet
    char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int alpha[27];
    for (int i = 0; i < 27; i++){
      alpha[i] = i;
    }

    // Initialize dec_message to be the same size as the plaintext message
    dec_message = malloc((strlen(fileBuffer) + 1) * sizeof(char));
    if (dec_message == NULL) {
      error("ERROR allocating memory for dec_message");
    }
    memset(dec_message, '\0', strlen(fileBuffer) + 1);

    //iterate through the key and file, adding the value of each letter to the dec_message
    int dec_message_index = 0;
    while (1){
      if (fileBuffer[dec_message_index] == '\0'){
        break;
      }
      char keyChar = keyBuffer[dec_message_index];
      char fileChar = fileBuffer[dec_message_index];
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
      int dec_index = (fileIndex - keyIndex);
      if (dec_index < 0) {
        dec_index += 27;
      }
      dec_message[dec_message_index++] = alphabet[dec_index];
    }

    dec_message[dec_message_index] = '\0';
    
    // Send the decoded message back to the client
    sendData(connectionSocket, dec_message);

    free(dec_message);

    // Close the connection socket for this client
    close(connectionSocket); 
  }

  // Close the listening socket
  close(listenSocket); 
  return 0;
}