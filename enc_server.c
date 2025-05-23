#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

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
  int total = 0;
  int bytesleft = strlen(buffer);
  int n;

  while (total < bytesleft){
    n = send(socketFD, buffer + total, bytesleft, 0);
    if (n < 0){
      error("SERVER: ERROR writing to socket");
    }
    total += n;
    bytesleft -= n;
  }
}

// Helper function to receive data
void receiveData(int socketFD, char* buffer, int bufferSize) {
  memset(buffer, '\0', bufferSize);
  int total = 0;
  int n;

  while (total < bufferSize - 1) {
    n = recv(socketFD, buffer + total, bufferSize - 1 - total, 0);
    if (n < 0) {
      error("SERVER: ERROR reading from socket");
    } else if (n == 0) {
      break; // Connection closed
    }
    total += n;

    // Check for newline character
    if (strchr(buffer, '\n') != NULL) {
      break;
    }
  }
}

void handleConnection(int connectionSocket) {
  char mainBuffer[80000];
  char fileBuffer[80000];
  char keyBuffer[80000];
  char* enc_message;

  memset(mainBuffer, '\0', sizeof(mainBuffer));
  memset(fileBuffer, '\0', sizeof(fileBuffer));
  memset(keyBuffer, '\0', sizeof(keyBuffer));

  char con_check[256];
  receiveData(connectionSocket, con_check, sizeof(con_check));

  //send a response to the client to let them know they are connected to the right server
  char* response = "ENC_SERVER\n";
  sendData(connectionSocket, response);

  // Receive the plaintext message from the socket
  receiveData(connectionSocket, mainBuffer, sizeof(mainBuffer));

  //separate the key and file from the buffer and put them into their own buffers
  char* token = strtok(mainBuffer, "|");
  strcpy(fileBuffer, token);
  token = strtok(NULL, "|");
  strcpy(keyBuffer, token);
  keyBuffer[strlen(keyBuffer) - 1] = '\0';

  // Assign a numerical value to the letters of the alphabet
  char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
  int alpha[27];
  for (int i = 0; i < 27; i++){
    alpha[i] = i;
  }

  // Initialize enc_message to be the same size as the plaintext message
  enc_message = malloc((strlen(fileBuffer) + 1) * sizeof(char));
  if (enc_message == NULL) {
    error("ERROR allocating memory for enc_message");
  }
  memset(enc_message, '\0', strlen(fileBuffer) + 1);

  //iterate through the key and file, adding the value of each letter to the enc_message
  int enc_message_index = 0;
  while (1){
    if (fileBuffer[enc_message_index] == '\0'){
      break;
    }
    char keyChar = keyBuffer[enc_message_index];
    char fileChar = fileBuffer[enc_message_index];
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
  strcat(enc_message, "\n");
  // Send the encoded message back to the client
  sendData(connectionSocket, enc_message);

  free(enc_message);

  // Close the connection socket for this client
  close(connectionSocket); 
}

int main(int argc, char *argv[]){
  int connectionSocket;
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

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
  listen(listenSocket, 100); 
  
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

    pid_t pid = fork();
    if (pid < 0) {
      error("SERVER: ERROR on fork");
    } else if (pid == 0) {
      // Child process
      close(listenSocket); // Close the listening socket in the child process
      handleConnection(connectionSocket);
      exit(0);
    } else {
      // Parent process
      close(connectionSocket); // Close the connection socket in the parent process
      waitpid(pid, NULL, 0); // Wait for the child process to complete
    }
  }

  // Close the listening socket
  close(listenSocket); 
  return 0;
}